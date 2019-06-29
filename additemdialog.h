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
    void on_addPushButton_clicked();
    void on_categoriesListView_clicked(const QModelIndex &index);
    void on_partsTableView_clicked(const QModelIndex &index);
    void on_colorsListView_clicked(const QModelIndex &index);
    void updateTotalCost();

signals:
    void insertItem(QMap<QString, QVariant> fields);

private:
    void setColorFilter(QString itemName = "");
    Ui::AddItemDialog *ui;
    QSqlTableModel *partsModel;
    QSqlTableModel *colorsModel;
    bool m_categorySelected = false;
    bool m_partSelected = false;
    bool m_colorSelected = false;
    int m_lastSelectedColor = -1;
};

#endif // ADDITEMDIALOG_H
