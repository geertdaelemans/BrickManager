#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QDialog>

namespace Ui {
class AddItemDialog;
}

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemDialog(QWidget *parent = nullptr);
    ~AddItemDialog();

private slots:
    void on_pushButton_clicked();

signals:
    void insertItem(QList<QString> fields);

private:
    Ui::AddItemDialog *ui;
};

#endif // ADDITEMDIALOG_H
