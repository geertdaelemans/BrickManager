#ifndef LABELSDIALOG_H
#define LABELSDIALOG_H

#include <QDialog>

#include <QSqlTableModel>

namespace Ui {
class LabelsDialog;
}

class LabelsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabelsDialog(QWidget *parent = nullptr, QString database = nullptr);
    ~LabelsDialog();

private slots:
    void refreshLabelList();
    void on_updatePushButton_clicked();
    void on_clearPushButton_clicked();

    void on_deletePushButton_clicked();

private:
    Ui::LabelsDialog *ui;
    QSqlTableModel *labelsModel;
    QString m_database;
};

#endif // LABELSDIALOG_H
