#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>

#include "curllabel.h"

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);

    virtual ~ProgressDialog() override;

    void setAutoClose(bool ac);
    void setHeaderText(const QString &str);
    void setMessageText(const QString &str);
    void setErrorText(const QString &str);
    void setProgressVisible(bool b);
    void setTextBlockVisible(bool b);
    void setTextBlock(const QString &str);
    void setFinished(bool ok);
    void setProgress(int steps, int total);

    void killAll();
    void syncRepaint(QWidget *w);

    void setSize();

protected slots:
    virtual void done(int r) override;

private:
    Ui::ProgressDialog *ui;
    CUrlLabel *m_message;
    QLabel *m_header;
    QProgressBar *m_progress;
    QWidget *m_progress_container;
    QTextEdit *m_textblock;
    QWidget *m_button_container;
    QPushButton *m_ok;
    QPushButton *m_cancel;

    QString m_message_text;
    bool m_message_progress : 1;
    bool m_autoclose        : 1;
    bool m_override         : 1;
};

#endif // PROGRESSDIALOG_H
