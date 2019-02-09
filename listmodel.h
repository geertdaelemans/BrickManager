#ifndef CATEGORIES_H
#define CATEGORIES_H

#include "datamodels.h"

#include <QDialog>
#include <QtSql>

namespace Ui {
class Categories;
}

class ListModel : public QDialog
{
    Q_OBJECT

public:
    explicit ListModel(QWidget *parent = nullptr, Tables table = Tables::userinventories);
    ~ListModel();
    enum Category {

    };

private slots:
    void slotCustomMenuRequested(QPoint);

private:
    void showError(const QSqlError &err);
    Ui::Categories *ui;
    TableModel *p_tableModel;
    void setVisibilityFromCheckBox();
    QSqlRelationalTableModel *model;
    QSortFilterProxyModel *proxyModel;
};

#endif // CATEGORIES_H