#include "ctransfer.h"

#include <QDebug>
#include <QDomDocument>
#include <QApplication>
#include <QtCore>
#include <QMessageBox>

#include "lzmadec.h"
#include "datamodel.h"
#include "sqldatabase.h"

bool CTransfer::s_global_init = false;
QMutex CTransfer::s_share_lock;
CURLSH *CTransfer::s_curl_share = nullptr;
uint CTransfer::s_instance_counter = 0;

/**
 * @brief
 *
 */
CTransfer::CTransfer()
{
    m_curl = nullptr;
	m_total = m_progress = 0;
	m_file_total = m_file_progress = 0;
	m_stop = false;

	m_use_proxy = false;
	m_proxy_port = 0;

    m_active_job = nullptr;

	s_instance_counter++;
}

/**
 * @brief
 *
 */
CTransfer::~CTransfer()
{
    qDebug("stopping CTransfer thread");

    cancelAllJobs();

    m_stop = true;
    m_queue_cond.wakeOne();
    wait ( );

    if(m_curl) {
        qDebug("********* Running the curl_easy_cleanup command *************");
        ::curl_easy_cleanup ( m_curl );
    }

    s_instance_counter--;

    if ( s_instance_counter == 0 ) {
        if ( s_curl_share ) {
            ::curl_share_cleanup ( s_curl_share );
            s_curl_share = nullptr;
        }

        if ( s_global_init ) {
            ::curl_global_cleanup ( );
            s_global_init = false;
        }
    }

}

//void CTransfer::setProxy ( bool enable, const QString &name, int port )
//{
//	m_queue_lock. lock ( );

//	m_use_proxy = enable;
//    m_proxy_name = name.toLatin1();
//	m_proxy_port = port;

//	m_queue_lock. unlock ( );
//}

/**
 * @brief
 *
 * @param QMap<QString
 * @param kvl
 * @return QByteArray
 */
QByteArray CTransfer::buildQueryString( const QMap<QString, QString> &kvl)
{
    QByteArray query;
    QMapIterator<QString, QString> it(kvl);
    while(it.hasNext()) {
        it.next();
        QByteArray qba1 = it.key().toLatin1();
        QByteArray qba2 = it.value().toLatin1();

        char *etmp1 = curl_escape(qba1.constData(), 0);
        char *etmp2 = curl_escape(qba2.constData(), 0);

        if(etmp1 && *etmp1) {
            if(!query.isEmpty())
                query += '&';
            query += etmp1;
            query += '=';

            if(etmp2 && *etmp2)
                query += etmp2;
        }
        curl_free(etmp2);
        curl_free(etmp1);
    }
    return query;
}

/**
 * @brief
 *
 * @param get
 * @param url
 * @param QMap<QString
 * @param query
 * @param ifnewer
 * @param file
 * @param userobject
 * @param high_priority
 * @return CTransfer::Job
 */
CTransfer::Job *CTransfer::retrieve(Job::httpMethod method, const QByteArray &url, const QMap<QString, QString> &query, Job::returnType returnType, QString name, bool tracking, time_t ifnewer, QFile *file, void *userobject, bool high_priority, Tables table)
{
    if(url.isEmpty() || (file && (!file->isOpen() || !file->isWritable())))
        return nullptr;

    Job *job = new Job();
    m_latestid++;
    job->m_id = m_latestid;
    if (tracking)
        m_tracker.insert(job->m_id, false);
    job->m_type = returnType;
    job->m_url = url;
    job->m_query = buildQueryString(query);
    job->m_ifnewer = ifnewer;

    job->m_name = name;
    job->m_table = table;
    job->m_data = file ? nullptr : new QByteArray ( );
    job->m_file = file;
    job->m_userobject = userobject;
    job->m_http_method = method;

    job->m_trans = this;
    job->m_result = CURLE_OK;
    job->m_respcode = 0;
    job->m_finished = false;
    job->m_filetime = 0;
    job->m_not_modified = false;
    job->m_failed = true;

    m_queue_lock.lock();

    if (high_priority)
        m_in_queue.prepend(job);
    else
        m_in_queue.append(job);
    m_queue_cond.wakeOne();

    qDebug("waking up thread... (%d in list)", m_in_queue.count());

    m_queue_lock.unlock();

    updateProgress(+1);

    return job;
}

/**
 * @brief Global initialization of curllib
 *
 * @return bool
 */
bool CTransfer::init()
{
    if (!s_global_init) {
        if (::curl_global_init(CURL_GLOBAL_WIN32) == CURLE_OK) {
            s_curl_share = ::curl_share_init();

            if (s_curl_share) {
                ::curl_share_setopt(s_curl_share, CURLSHOPT_LOCKFUNC, lock_curl);
                ::curl_share_setopt(s_curl_share, CURLSHOPT_UNLOCKFUNC, unlock_curl);
                ::curl_share_setopt(s_curl_share, CURLSHOPT_USERDATA, 0);
                ::curl_share_setopt(s_curl_share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
                s_global_init = true;
            }
        }
    }

    if (s_global_init)
        m_curl = ::curl_easy_init();

    if (m_curl) {
        ::curl_easy_setopt(m_curl, CURLOPT_SHARE, s_curl_share);
        start(IdlePriority);
    }


    connect(this, SIGNAL(progress(CTransfer::Job*, int, int)), this, SLOT(transferProgress(CTransfer::Job *, int, int)));
    connect(this, SIGNAL(finished(CTransfer::Job*)), this, SLOT(transferDone(CTransfer::Job*)));
    connect(this, SIGNAL(finished(CTransfer::Job*)), this, SLOT(gotten(CTransfer::Job*)));

    return(m_curl);
}

/**
 * @brief CTransfer::run
 *
 */

void CTransfer::run()
{
//	QString ua = cApp-> appName ( ) + "/" +
//	             cApp-> appVersion ( ) + " (" +
//	             cApp-> sysName ( ) + " " +
//				 cApp-> sysVersion ( ) + "; http://" +
//	             cApp-> appURL ( ) + ")";
    QString ua = "BrickStore/1.2.12 (Windows 8; http://www.brickforge.de/software/brickstore)";
    QFile capath(":/cacert.pem");
    qDebug("CTransfer::run()");
    ::curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, "cookies.txt");
    ::curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, "");
    ::curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 0);
    ::curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0);
    ::curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, progress_curl);
    ::curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, this);
    ::curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1);
    ::curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1);
//	::curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 120 );
    ::curl_easy_setopt(m_curl, CURLOPT_DNS_CACHE_TIMEOUT, 5*60);
    ::curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_curl);
    ::curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
    ::curl_easy_setopt(m_curl, CURLOPT_USERAGENT, ua.toLatin1().data());
    ::curl_easy_setopt(m_curl, CURLOPT_ENCODING, "");
    ::curl_easy_setopt(m_curl, CURLOPT_FILETIME, 1);
    ::curl_easy_setopt(m_curl, CURLOPT_SSLCERTTYPE, "PEM");
    ::curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 1L);
//  ::curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, FALSE);
    ::curl_easy_setopt(m_curl, CURLOPT_CAINFO, "d:\\cacert.pem");
    ::curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, "cookies.txt");
    QString url;
    QByteArray query;

    while(!m_stop) {
        Job *job = nullptr;

        m_queue_lock.lock();
        m_active_job = nullptr;

        if (m_in_queue.isEmpty()) {
            m_queue_cond.wait(&m_queue_lock);
            qDebug() << "Empty queue";
        }

        if (!m_in_queue.isEmpty()) {
            job = m_in_queue.takeAt(0);
            qDebug() << "Found task in queue";
        }

        if(job) {
            m_active_job = job;
            if (m_use_proxy) {
                ::curl_easy_setopt(m_curl, CURLOPT_PROXY, m_proxy_name.data());
                ::curl_easy_setopt(m_curl, CURLOPT_PROXYPORT, m_proxy_port);
            }
            else
                ::curl_easy_setopt(m_curl, CURLOPT_PROXY, 0);

            qDebug("Running job: %i", job->m_id);

            if (job->m_http_method == Job::Get) {

                // Prepare GET URL
                url = job->m_url;
                if (!job->m_query.isEmpty()) {
                    url += '?';
                    url += job->m_query;
                }
                query = "";

                ::curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1);
                ::curl_easy_setopt(m_curl, CURLOPT_URL, url.toLocal8Bit().data());
                ::curl_easy_setopt(m_curl, CURLOPT_TIMEVALUE, job->m_ifnewer);
                ::curl_easy_setopt(m_curl, CURLOPT_TIMECONDITION, job->m_ifnewer ? CURL_TIMECOND_IFMODSINCE : CURL_TIMECOND_NONE);
                qDebug("CTransfer::get [%s]", url.toLocal8Bit().data());
            }
            else {

                // Prepare POST URL
                url = job->m_url;
                query = job->m_query;

                ::curl_easy_setopt(m_curl, CURLOPT_POST, 1);
                ::curl_easy_setopt(m_curl, CURLOPT_URL, url.toLocal8Bit().data());
                ::curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, query.data());
                ::curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, job->m_query.length());
                qDebug("CTransfer::post [%s] - form-data [%s]", url.toLocal8Bit().data(), query.data());
            }
        }

        m_queue_lock.unlock();

        if (m_stop)
            break;
        if (!job)
            continue;

        CURLcode res = ::curl_easy_perform(m_curl);

        curl_easy_setopt(m_curl, CURLOPT_COOKIELIST, "FLUSH");

        long respcode = 0;
        char *effurl = nullptr;
        long filetime = -1;

        m_file_total = m_file_progress = -1;
        QApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>(TransferProgressEvent)));

        if (res == CURLE_OK) {
            ::curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &respcode);
            ::curl_easy_getinfo(m_curl, CURLINFO_EFFECTIVE_URL, &effurl);
            ::curl_easy_getinfo(m_curl, CURLINFO_FILETIME, &filetime);
        }

        m_queue_lock.lock();

        if (m_active_job == job) {
            job->m_result = res;
            if (res != CURLE_OK)
                job->m_error = ::curl_easy_strerror(res);
            job->m_respcode = respcode;
            job->m_effective_url = effurl;
            job->m_filetime = time_t((filetime != -1) ? filetime : 0);

            if (job->m_ifnewer && ((respcode == 304) || (filetime != -1 && filetime < job->m_ifnewer)))
                job->m_not_modified = true;

            job->m_failed = (res != CURLE_OK) || !((respcode == 200) || (respcode == 304));
        }
        else if (m_active_job == nullptr) {
            job->m_result = CURLE_ABORTED_BY_CALLBACK;
            job->m_error = "Aborted";
            job->m_respcode = 0;
            job->m_failed = true;
        }
        job->m_finished = true;
        m_out_queue.append(job);

        m_active_job = nullptr;
        m_queue_lock.unlock();

        QApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>(TransferFinishedEvent)));
    }
}

/**
 * @brief
 *
 * @param event
 */
void CTransfer::customEvent(QEvent *event)
{
    if (int(event->type()) == TransferFinishedEvent) {
        // we have to copy the job list to avoid problems with
        // recursive mutex locking (we emit signals, which
        // can lead to slots calling another CTransfer function)
        qDebug("TransferFinishedEvent");
        QList<Job *> finish;

        m_queue_lock.lock();

        while (!m_out_queue.isEmpty())
            finish.append(m_out_queue.takeAt(0));

        m_queue_lock.unlock();

        while (!finish.isEmpty()) {
            Job *j = finish.takeAt(0);
            qDebug("Emitted finished");
            emit finished(j);
            updateProgress(-1);

            delete j->file();
            delete j->data();
            delete j;
        }
    }
    else if (int(event->type()) == TransferProgressEvent) {
        MyCustomEvent* custom_event = reinterpret_cast<MyCustomEvent*>(event);
        Job *j = custom_event->active_job;
        if (j)
            emit progress(j, m_file_progress, m_file_total);
    }
}

/**
 * @brief
 *
 * @param delta
 */
void CTransfer::updateProgress(int delta)
{
    if(delta > 0)
        m_total += delta;
    else
        m_progress -= delta;
    emit progress(m_progress, m_total);

    if(m_total == m_progress) {
        m_total = m_progress = 0;
        qDebug("Emit Done");
        emit done();
        emit progress(0, 0);
    }
}

/**
 * @brief Cancel one specific job
 *
 * @param job job to be cancelled
 */
void CTransfer::cancel(Job *job)
{
    job->m_result = CURLE_ABORTED_BY_CALLBACK;
    job->m_respcode = 0;
    job->m_finished = true;

    emit finished(job);

    updateProgress(-1);

    delete job->file();
    delete job->data();
    delete job;
}

/**
 * @brief Cancel all pending jobs
 *
 */
void CTransfer::cancelAllJobs()
{
    qDebug("CTransfer::cancelAllJobs()");
    // we have to copy the job list to avoid problems with
    // recursive mutex locking (cancel emits signals, which
    // can lead to slots calling another CTransfer function)
    QList<Job*> canceled;

    m_queue_lock.lock();

    while (!m_out_queue.isEmpty())
        canceled.append(m_out_queue.takeAt(0));

    if (m_active_job) {
        // we can't delete the Job data right away, since curl_write may be accessing it at any time
        //canceled. append ( m_active_job );

        // just tell the worker thread, that the active job doesn't matter anymore...
        m_active_job = nullptr;
    }

    while (!m_in_queue.isEmpty())
        canceled.prepend(m_in_queue.takeAt(0));

    m_queue_lock.unlock();

    while (!canceled.isEmpty())
        cancel(canceled.takeAt(0));

//    ::curl_easy_cleanup(m_curl);
}

/**
 * @brief Callback function for writing received data (curl)
 *
 * This callback function gets called by libcurl as soon as there is data received that needs to be saved.
 * For most transfers, this callback gets called many times and each invoke delivers another chunk of data.
 * ptr points to the delivered data, and the size of that data is nmemb; size is always 1.
 *
 * @param ptr Pointer to delivered data.
 * @param size Size is always 1.
 * @param nmemb Size of the delivered data.
 * @param stream Contains pointer to original object (as defined in CURLOPT_WRITEDATA).
 * @return size_t The number of bytes actually taken care of.
 */
size_t CTransfer::write_curl(void *ptr, size_t size, size_t nmemb, void *stream)
{
    CTransfer *that = static_cast<CTransfer *>(stream);
    Job *j = that->m_active_job;

    if (j && j->m_data) {
        int oldsize = j->m_data->size();
        int newSize = static_cast<int>(size * nmemb);
        that->m_bytesReceived += newSize;
        j->m_data->resize(oldsize + newSize);
        ::memcpy(j->m_data->data() + oldsize, ptr, size * nmemb);
        return size * nmemb;
    }
    else if (j && j->m_file) {
        qint64 res = j->m_file->write(static_cast<const char *>(ptr), size * nmemb);
        that->m_bytesReceived += size * nmemb;
        if (res < 0) {
            // set error code
            res = 0;
        }
        return static_cast<size_t>(res);
    }
    else
        return 0;
}

int CTransfer::progress_curl(void *stream, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/)
{
    CTransfer *that = static_cast<CTransfer *>(stream);
    that->m_file_progress = int(dlnow);
    that->m_file_total = int(dltotal);

    MyCustomEvent* event = new MyCustomEvent(QEvent::Type(TransferProgressEvent));
    event->active_job = that->m_active_job;
    QApplication::postEvent(that, reinterpret_cast<QEvent*>(event));
    return 0;
}

void CTransfer::lock_curl(CURL * /*handle*/, curl_lock_data /*data*/, curl_lock_access /*access*/, void * /*userptr*/)
{
    s_share_lock.lock();
}

void CTransfer::unlock_curl(CURL * /*handle*/, curl_lock_data /*data*/, void * /*userptr*/)
{
    s_share_lock.unlock();
}


/**
 * @brief
 *
 * @param j
 * @param s
 * @param t
 */
void CTransfer::transferProgress(CTransfer::Job *j, int s, int t)
{
    if (j && (j == m_job) && m_progressDialog)
        m_progressDialog->setProgress(s/1024, t/1024);
}


/**
 * @brief
 *
 * @param j
 */
void CTransfer::transferDone(CTransfer::Job *j)
{
    qDebug() << QString("CTransfer::transferDone ID%1").arg(j->m_id);

    if (j && j->file() && j->file()->isOpen()) {
        j->file()->close();
        qDebug() << "emit jobDone() for ID" << j->m_id;
        emit jobDone(j);
        qDebug() << "Closed" << j->file()->fileName();
    }

    if (j->failed() && m_progressDialog)
        m_progressDialog->setErrorText(tr("Download failed: %1").arg(j->errorString()));
}

/**
 * @brief Check if more jobs need to be completed after the current one.
 *
 * A tracker m_tracker keeps a list of all jobs that need to be executed.
 * When a job is finished the tracker is updated. When all jobs are finished,
 * this function returns true.
 *
 * @param job Current job that is finished
 * @return bool True if no more jobs need to be started, False if there are
 */
bool CTransfer::isJobCompleted(Job *job)
{
    if (m_tracker.contains(job->m_id)) {                // Check if job is being tracked
        m_tracker[job->m_id] = true;                    // Set the currect job as completed in tracker
        QMapIterator<int, bool> iterator(m_tracker);    // Check is any job is still unfinished
        while (iterator.hasNext()) {
            iterator.next();
            if (!iterator.value()) {
                return false;
            }
        }
        m_tracker.clear();                              // Clear tracker when all tasks are complete
    }
    return true;
}

void CTransfer::brickLinkLogin(ProgressDialog *pd)
{
    // When a pointer to a ProgressDialog is given, prepare the Dialog
    m_progressDialog = pd;
    if (m_progressDialog) {
        m_progressDialog->setAutoClose(false);
        m_progressDialog->setProgressVisible(false);
        m_progressDialog->setWindowTitle(tr("Logging in"));
        m_progressDialog->setHeaderText(tr("Logging in"));
    }

    // Login URL for BrickLink
    const char *url = "https://www.bricklink.com/ajax/renovate/loginandout.ajax";

    // Prepare query based upon settings in the registry
    QMap<QString, QString> query;
    QSettings settings;
    query["userid"] = settings.value("bricklinklogin/userName").toString();
    query["password"] = settings.value("bricklinklogin/passWord").toString();
    query["keepme_loggedin"] = "true";
    query["override"] = "false";

    // Submit the POST request
    m_job = retrieve(Job::Post, url, query, Job::Login, "login");

    // Next just call "catalogDownload to update the cookie.
    // This call returns an error page, but this can be ignored.
    // No clue why this is working, but it does...

    // Login URL for BrickLink
    url = "http://www.bricklink.com/catalogDownload.asp";

    // Submit the POST request
    m_job = retrieve(Job::Post, url, query, Job::NoReturn, "login", false);

}

void CTransfer::importCatalog(ProgressDialog *pd)
{
    m_progressDialog = pd;
    if (m_progressDialog) {
        m_progressDialog->setAutoClose(false);
        m_progressDialog->setProgressVisible(true);
        m_progressDialog->setTextBlockVisible(true);
        m_progressDialog->setTextBlock(tr("Starting downloads..."));
        m_progressDialog->setWindowTitle(tr("Importing BrickLink Catalogs"));
        m_progressDialog->setHeaderText(tr("Importing BrickLink Catalogs"));
        m_progressDialog->setMessageText(tr("Download: %1/%2 KB"));
        m_progressDialog->layout();
    }

    const char *url = "https://www.bricklink.com/catalogDownload.asp";

    char typeCode[] = {'S', 'P', 'M', 'B', 'G', 'C', 'I', 'O'};
    QString typeName[] = {"sets", "parts", "minifigs", "books", "gear", "catalogs", "instructions", "boxes"};
    Tables tableName[] = {Tables::sets, Tables::parts, Tables::minifigs, Tables::books, Tables::gear, Tables::catalogs, Tables::instructions, Tables::boxes};

    QMap<QString, QString> query;
    query["viewType"] = "0";            // 0: Catalog Items, 1: Item Types, 2: Categories, 3: Colors, 4: Inventory, 5: Part and Color Codes
    query["itemType"] = "P";            // S: Sets, P: Parts, M: Minifigs, B: Books, G: Gear, C: Catalogs, I: Instructions, O: Original Boxes
    query["selYear"] = "Y";             // Include Year
    query["selWeight"] = "Y";           // Include Weight
    query["selDim"] = "Y";              // Include Dimensions
    query["itemTypeInv"] = "S";         // S: Set, P: Part, M: Minifig, B: Book, G: Gear
    query["itemNo"] = "";               // Item number
    query["downloadType"] = "X";        // T: Tab-Delimited File, X: XML

    // Process categories
    query["viewType"] = "2";            // Categories
    m_job = retrieve(Job::Post, url, query, Job::PartColor, "categories", true, 0, nullptr, nullptr, false, Tables::categories);

    // Process all catalogs
    query["viewType"] = "0";            // Catalog Items
    for (int i = 0; i < static_cast<int>(sizeof(typeCode)/sizeof(*typeCode)) ; i++) {
        query["itemType"] = typeCode[i];
        m_job = retrieve(Job::Post, url, query, Job::PartColor, typeName[i], true, 0, nullptr, nullptr, false, tableName[i]);
    }

    // Process partcolor
    query["viewType"] = "5";
    m_job = retrieve(Job::Post, url, query, Job::PartColor, "partcolor", true, 0, nullptr, nullptr, false, Tables::partcolor);
}


void CTransfer::gotten(CTransfer::Job* job)
{
    // LOGIN SEQUENCE
    if (job->m_type == Job::Login) {
        QByteArray *data = job->data();

        if (data && data->size()) {
            QJsonParseError parseError;
            const auto document = QJsonDocument::fromJson(*data, &parseError);
            if(!parseError.error) {
                QJsonObject jsonObject = document.object();
                if(m_progressDialog) {
                    m_progressDialog->setMessageText(jsonObject["returnMessage"].toString());
                    m_progressDialog->setTextBlockVisible(true);
                    foreach (const QString key, jsonObject.keys()) {
                        if (jsonObject[key].isString())
                            m_progressDialog->setTextBlock(tr("%1: %2").arg(key).arg(jsonObject[key].toString()));
                        else
                            m_progressDialog->setTextBlock(tr("%1: %2").arg(key).arg(jsonObject[key].toInt()));
                    }
                } else {
                    switch(jsonObject["returnCode"].toInt()) {
                    case 0:
                        emit updateStatusBar(tr("Login succesful"), 3000);
                        break;
                    case -4:
                        emit updateStatusBar(tr("Already logged-in"), 3000);
                        break;
                    case -11:
                        emit updateStatusBar(tr("ID or Password Mismatch!"), 3000);
                        break;
                    default:
                        emit updateStatusBar(jsonObject["returnCode"].toString());
                    }
                }
            } else {
                QMessageBox msgBox;
                msgBox.setText(parseError.errorString());
                msgBox.exec();
            }
        }
        if (isJobCompleted(job) && m_progressDialog) {
            m_progressDialog->setFinished(true);
            m_bytesReceived = 0;
        }
    }

    // TAB DELIMITED INPUT
    else {
        if (job->m_data) {

            // Testing for Log-in OK
            if (job->m_data->mid(2, 15) == "<!doctype html>") {
                if (m_progressDialog) {
                    m_progressDialog->setMessageText(tr("Error logging in."));
                    m_progressDialog->setTextBlock(tr("Either your username or password are incorrect."));
                }
                return;
            }

            QString category = job->m_name;
            if (m_progressDialog) {
                m_progressDialog->setMessageText(tr("Finished importing %1.").arg(category));
                m_progressDialog->setTextBlock(tr("Downloaded %1 (%2 kB).").arg(category).arg(QString::number(job->m_data->size()/1024.0, 'f', 1)));
            }
            populateDatabase(job);
        }
        if (isJobCompleted(job)) {
            m_progressDialog->setMessageText(tr("Completed downloading."));
            m_progressDialog->setTextBlock(tr("Completed downloading %1 kB.").arg(QString::number(m_bytesReceived/1024.0, 'f', 1)));
            m_progressDialog->setFinished(true);
            m_progressDialog->setProgressVisible(false);
            m_bytesReceived = 0;
        }
    }
}

int CTransfer::populateDatabase(CTransfer::Job* job)
{
    int counter = 0;
    QByteArray *data = job->m_data;
    QString job_name = job->m_name;
    Tables table = job->m_table;
    QString sqlTableName = job->m_name;

    if (data && data->size()) {

        // Prepare transaction
        QSqlDatabase::database("catalogDatabase").transaction();

        DataModel *p_dataModel = new DataModel(table);
        sqlTableName = p_dataModel->getSqlTableName();

        // Delete table if exists
        QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
        q.exec("DROP TABLE IF EXISTS " + sqlTableName);

        // Create table
        p_dataModel->initiateSqlTableAuto("catalogDatabase");

        // Get translation table from XML tags to SQL columns
        QMap<QString, QString> translationTable = p_dataModel->getTranslationTable();
        translationTable.remove("id");

        // Prepare query string
        QString qryString = "INSERT INTO ";
        qryString += p_dataModel->getSqlTableName() + " (";
        bool first = true;
        QString tempString;
        for (QString key : translationTable.keys()) {
            if (first) {
                qryString += translationTable[key];
                tempString += ":" + translationTable[key];
                first = false;
            } else {
                qryString += ", " + translationTable[key];
                tempString += ", :" + translationTable[key];
            }
        }
        qryString += ") VALUES (" + tempString + ")";

        // Prepare query
        if (!q.prepare(qryString)) {
            qDebug() << "Failed to add item to database" << p_dataModel->getSqlTableName() << q.lastError();
            m_progressDialog->setMessageText(tr("Failed to add item to database."));
            return 0;
        }

        // Parse XML data
        QBuffer *store_buffer = new QBuffer(data);
        if (store_buffer->open(QIODevice::ReadOnly)) {
            QString emsg;
            int eline = 0, ecol = 0;
            QDomDocument doc;
            if (doc.setContent(store_buffer, &emsg, &eline, &ecol)) {
                QDomElement root = doc.documentElement();
                QDomNodeList itemList = root.childNodes(); // CODES level
                qDebug() << "path" << root.tagName() << itemList.at(0).toElement().tagName();
                for (int i = 0; i < itemList.size(); i++){
                    QDomNode activeNode = itemList.at(i);  // ITEM level
                    for(QString tagName : translationTable.keys()) {
                        q.bindValue(":" + translationTable[tagName], activeNode.firstChildElement(tagName).text());
                    }
                    q.exec();
                    counter++;
                }
            }
            else {
                if ((QByteArray(data->data(), 15) == "<!doctype html>") && (QByteArray(data->data(), data->size()).indexOf("Error!", 1) != -1 )) {
                    qDebug("Either your username or password are incorrect.");
                    if (m_progressDialog)
                        m_progressDialog->setErrorText(tr("Either your username or password are incorrect."));
                }
                else {
                    QString errorMessage = QString("Could not parse the XML data for the store inventory:<br /><i>Line %1, column %2: %3</i>" ).arg(eline).arg(ecol).arg(emsg);
                    qDebug() << errorMessage;
                    if (m_progressDialog)
                        m_progressDialog->setErrorText(errorMessage);
                }
            }
        }

        QSqlDatabase::database("catalogDatabase").commit();
    }

    // Update the categories database with catalog information
    if (table != Tables::partcolor) {
        QSqlDatabase::database("catalogDatabase").transaction();
        QString queryString;
        queryString = "SELECT DISTINCT category_id FROM " + sqlTableName;
        QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
        if (!q.prepare(queryString))
            qDebug() << q.lastError();
        q.exec();
        while (q.next()) {
            int catNo = q.value(0).toInt();
            QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
            if (!q.prepare(QString("UPDATE categories SET %1 = 1 WHERE id == %2").arg(sqlTableName).arg(catNo)))
                qDebug() << q.lastError();
            q.exec();
        }
        QSqlDatabase::database("catalogDatabase").commit();
    }

    m_progressDialog->setMessageText(tr("Populated %1 database.").arg(sqlTableName));
    m_progressDialog->setTextBlock(tr("Populated %1 database with %2 records.").arg(sqlTableName).arg(counter));

    return counter;
}
