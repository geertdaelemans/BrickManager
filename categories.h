#ifndef CATEGORIES_H
#define CATEGORIES_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class Categories;
}

class Categories : public QDialog
{
    Q_OBJECT

public:
    explicit Categories(QWidget *parent = nullptr);
    ~Categories();

private:
    void showError(const QSqlError &err);
    Ui::Categories *ui;
    QSqlRelationalTableModel *model;
};

#endif // CATEGORIES_H
