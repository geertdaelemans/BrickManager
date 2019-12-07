#ifndef CATEGORIES_H
#define CATEGORIES_H

#include "datamodel.h"
#include "generictablemodel.h"

#include <QDialog>
#include <QtSql>

namespace Ui {
class Categories;
}

class ListModel : public QDialog
{
    Q_OBJECT

public:
    explicit ListModel(QWidget *parent = nullptr, DataModel *tableModel = nullptr, QSqlDatabase database = QSqlDatabase());
    ~ListModel() override;
    bool insertRow(QMap<QString, QVariant> fields);
    void refresh();

private slots:
    void slotCustomMenuRequested(QPoint);

private:
    void showError(const QSqlError &err);
    Ui::Categories *ui;
    DataModel *p_dataModel;
    void setVisibilityFromCheckBox();
    GenericTableModel *model;
    QSortFilterProxyModel *proxyModel;
};

#endif // CATEGORIES_H
