#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QNetworkReply>

#include "bricklink.h"

namespace Ui {
class AddItemDialog;
}

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemDialog(QWidget *parent = nullptr);
    ~AddItemDialog();
    void getImage(QString part, QString itemType = "part");

private slots:
    void updateCategories(QString category = "parts");
    void statusAddButton();
    void on_addPushButton_clicked();
    void on_categoriesListView_clicked(const QModelIndex &index);
    void on_partsTableView_clicked(const QModelIndex &index);
    void on_colorsListView_clicked(const QModelIndex &index);
    void updateTotalCost();
    void finishedSlot(QNetworkReply* reply);

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
    BrickLink::ItemType* m_category;

    QImage * image;
    QPixmap screenImage;
    QPainter * p;
    QBuffer *buffer;
    QByteArray bytes;
    int Request;
    QNetworkAccessManager* nam;
};

#endif // ADDITEMDIALOG_H
