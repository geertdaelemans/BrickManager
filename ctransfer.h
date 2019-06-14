#ifndef __CTRANSFER_H__
#define __CTRANSFER_H__

#include <time.h>
#include <progressdialog.h>
#include <datamodel.h>

#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qpair.h>
#include <qdatetime.h>
#include <QEvent>
#include <QFile>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <curl/curl.h>

typedef QPair<QString, QString> CKeyValue;
//typedef QList<CKeyValue> CKeyValueList;
typedef QMap<QString, QString> CKeyValueList;


class CTransfer : public QThread {

    Q_OBJECT
public:
    class Job {
    public:
        QString url() const              { return m_url; }
        QByteArray effectiveUrl() const  { return m_effective_url; }
        QString errorString() const      { return failed ( ) ? m_error : QString(); }
        bool failed() const              { return m_finished && m_failed; }
        int responseCode() const         { return m_respcode; }
        bool finished ( ) const          { return m_finished; }
        QFile *file ( ) const            { return m_file; }
        QByteArray *data ( ) const       { return m_data; }
        void *userObject ( ) const       { return m_userobject; }
        QDateTime lastModified ( ) const { QDateTime d; d.setTime_t ( m_filetime ); return d; }
        bool notModifiedSince ( ) const  { return m_not_modified; }
        enum returnType {Login, Catalog, PartColor, NoReturn};
        enum httpMethod {Get, Post};

    private:
        friend class CTransfer;
        Job ( ) { }

    private:
        CTransfer *  m_trans;
        int          m_id;
        returnType   m_type;
        QByteArray   m_url;
        QByteArray   m_query;
        QByteArray   m_effective_url;
        QString      m_name;
        Tables       m_table;
        QByteArray * m_data;
        QFile *      m_file;
        QString      m_error;
        void *       m_userobject;
        time_t       m_ifnewer;
        time_t       m_filetime;

        CURLcode     m_result;
        int          m_respcode     : 16;
        bool         m_finished     : 1;
        httpMethod   m_http_method  = Job::Get;
        bool         m_not_modified : 1;
        bool         m_failed       : 1;
    };

public:
    class MyCustomEvent : public QEvent
    {
    public:
        Job *active_job;
        MyCustomEvent(QEvent::Type type) : QEvent(type)
        {
        }
    };

public:
    CTransfer();
    virtual ~CTransfer() override;

    bool init();

    void brickLinkLogin(ProgressDialog *pd = nullptr);
    void importCatalog(ProgressDialog *pd = nullptr);

public slots:
    void cancelAllJobs();
//	void setProxy ( bool enable, const QString &name, int port );

signals:
    void progress(int total, int progress);
    void progress(CTransfer::Job *, int total, int progress);
    void finished(CTransfer::Job *);
    void updateStatusBar(QString msg, int timeout = 0);
    void done();

protected:
    enum {
        TransferStartedEvent  = QEvent::User + 0x42,
        TransferFinishedEvent = QEvent::User + 0x43,
        TransferProgressEvent = QEvent::User + 0x44
    };

    virtual void customEvent(QEvent *) override;

private:
    virtual void run() override;
    Job *retrieve(Job::httpMethod method, const QByteArray &url, const QMap<QString, QString> &query, Job::returnType returnType, QString name, bool tracking = false, time_t ifnewer = 0, QFile *file = nullptr, void *userobject = nullptr, bool high_priority = false, Tables table = Tables::parts);
    void cancel(Job *j);
    void updateProgress(int delta);

    static QByteArray buildQueryString(const QMap<QString, QString> &kvl);
	
    static size_t write_curl ( void *ptr, size_t size, size_t nmemb, void *stream );
    static int progress_curl ( void *clientp, double dltotal, double dlnow, double ultotal, double ulnow );

    static void lock_curl(CURL * /*handle*/, curl_lock_data /*data*/, curl_lock_access /*access*/, void * /*userptr*/);
    static void unlock_curl(CURL * /*handle*/, curl_lock_data /*data*/, void * /*userptr*/);

    int populateDatabase(CTransfer::Job* job);

    bool isJobCompleted(Job *j);

private:
    QWidget* m_parent;
	CURL *m_curl;
    int   m_latestid = 0;
    int   m_total;
    int   m_progress;
    int   m_file_total;
    int   m_file_progress;
    bool  m_stop;
    Job *m_job;
    QMap<int, bool> m_tracker;

    int m_bytesReceived = 0;

    bool     m_use_proxy;
    QByteArray m_proxy_name;
    int      m_proxy_port;

    Job *          m_active_job;
    QList<Job*>    m_in_queue;
    QList<Job*>    m_out_queue;
    QMutex         m_queue_lock;
    QWaitCondition m_queue_cond;

    static uint    s_instance_counter;
    static bool    s_global_init;
    static QMutex  s_share_lock;
    static CURLSH *s_curl_share;

    ProgressDialog *m_progressDialog;

private slots:
    virtual void gotten(CTransfer::Job*);
    void transferProgress(CTransfer::Job *job, int, int);
    void transferDone(CTransfer::Job *job);

signals:
    void jobDone(CTransfer::Job *job);
};

#endif
