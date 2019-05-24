#include "progressdialog.h"
#include "ui_progressdialog.h"

#include <QBoxLayout>
#include <QPushButton>

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);

    m_autoclose = true;

    m_message_progress = false;

    int minwidth = fontMetrics().width('m') * 40;

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setMargin(11);
    lay->setSpacing(6);

    m_header = new QLabel(this);
    m_header->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_header->setMinimumWidth(minwidth);
    lay->addWidget(m_header);

    QFrame *frame = new QFrame(this);
    frame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    lay->addWidget(frame);

    m_message = new CUrlLabel(this);
    m_message->setAlignment(Qt::AlignCenter);
    m_message->setFixedHeight(20);
    lay->addWidget(m_message);

    m_progress_container = new QWidget(this);
    lay->addWidget(m_progress_container);

    QHBoxLayout *play = new QHBoxLayout(m_progress_container);
    play->setMargin(0);
    play->setSpacing(6);

    m_progress = new QProgressBar(m_progress_container);
    m_progress->setTextVisible(false);
    play->addSpacing(20);
    play->addWidget(m_progress);
    play->addSpacing(20);

    m_textblock = new QTextEdit(m_progress_container);
    m_textblock->setFixedHeight(100);
    m_textblock->hide();
    lay->addWidget(m_textblock);

    frame = new QFrame(this);
    frame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    lay->addWidget(frame);

    m_button_container = new QWidget(this);
    lay->addWidget(m_button_container);

    QHBoxLayout *blay = new QHBoxLayout(m_button_container);
    blay->addSpacing(11);
    blay->addStretch(10);

    m_ok = new QPushButton(tr("&OK"), this);
    blay->addWidget(m_ok);
    m_ok->hide();

    m_cancel = new QPushButton(tr("&Cancel"), this);
    blay->addWidget(m_cancel);

    blay->addStretch(10);
    blay->addSpacing(11);

    connect(m_cancel, SIGNAL(clicked()), this, SLOT(reject()));

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_override = true;

    setLayout(lay);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

/**
 * @brief
 *
 * @param r
 */
void ProgressDialog::done(int r)
{
//    if (m_trans)
//        m_trans->cancelAllJobs();

    QDialog::done(r);

    if (m_override)
        QApplication::restoreOverrideCursor();
}


void ProgressDialog::killAll()
{
//    if (m_trans)
//        m_trans->cancelAllJobs();

    if (m_override)
        QApplication::restoreOverrideCursor();
}

/**
 * @brief
 *
 * @param str
 */
void ProgressDialog::setHeaderText(const QString &str)
{
    m_header->setText(QString("<b>%1</b>").arg(str));
    syncRepaint(m_header);
}

/**
 * @brief
 *
 * @param str
 */
void ProgressDialog::setMessageText(const QString &str)
{
    m_message_text = str;
    m_message_progress = (str.contains("%1") && str.contains("%2"));

    if(m_message_progress)
        setProgress(0, 0);
    else
        m_message->setText(str);

    syncRepaint(m_message);
}


/**
 * @brief
 *
 * @param str
 */
void ProgressDialog::setErrorText(const QString &str)
{
    m_message->setText(QString("<b>%1</b>: %2").arg(tr("Error")).arg(str));
    setFinished(false);

    syncRepaint(m_message);
}

/**
 * @brief
 *
 * @param ok
 */
void ProgressDialog::setFinished(bool ok)
{
    QApplication::restoreOverrideCursor();
    m_override = false;

    m_progress-> setMaximum(100);
    m_progress-> setValue(100);

    if ( m_autoclose && ok ) {
        accept();
    }
    else {
        m_cancel->hide();
        m_ok->show();
        connect(m_ok, SIGNAL(clicked()), this, ok ? SLOT(accept()) : SLOT(reject()));
        setSize();
    }
}


/**
 * @brief
 *
 * @param steps
 * @param total
 */
void ProgressDialog::setProgress(int steps,int total)
{
    m_progress->setValue(steps);
    m_progress->setMaximum(total);

    if(m_message_progress) {
        QString str = m_message_text.arg(steps);
        if (total)
            str = str.arg(total);
        else
            str = str.arg("?");
        m_message->setText(str);
    }
}

void ProgressDialog::setProgressVisible(bool b)
{
    m_progress_container->setVisible(b);
}

void ProgressDialog::setTextBlock(const QString &str)
{
    m_textblock->append(str);

    syncRepaint(m_textblock);
}

void ProgressDialog::setTextBlockVisible(bool b)
{
    m_textblock->setVisible(b);
}

/**
 * @brief
 *
 * @param w
 */
void ProgressDialog::syncRepaint(QWidget *w)
{
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    layout();
    w->repaint();
    w->updateGeometry();
    setSize();
}

/**
 * @brief
 *
 */
void ProgressDialog::setSize()
{
    setFixedSize(sizeHint());
}

void ProgressDialog::setAutoClose(bool ac)
{
    m_autoclose = ac;
}
