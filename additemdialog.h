#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QDialog>
#include <QtSql>

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
    void updateCategories(QString category = "parts");
    void statusAddButton();

private slots:
    void on_addPushButton_clicked();

    void on_categoriesListView_clicked(const QModelIndex &index);

    void on_partsTableView_clicked(const QModelIndex &index);

    void on_colorsListView_clicked(const QModelIndex &index);

signals:
    void insertItem(QList<QString> fields);

private:
    Ui::AddItemDialog *ui;
    QSqlTableModel *partsModel;
    bool m_categorySelected = false;
    bool m_partSelected = false;
    bool m_colorSelected = false;
};

#endif // ADDITEMDIALOG_H
