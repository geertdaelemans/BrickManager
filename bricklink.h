#ifndef BRICKLINK_H
#define BRICKLINK_H

#include <QtNetworkAuth>

class BrickLink : public QOAuth1
{
    Q_OBJECT

public:
    BrickLink(QObject *parent = nullptr);
    bool checkConnection(QObject *parent = nullptr);
    void importColors();
    void importOrderItem(int orderID);
    void importOrders(bool filed);
    void importUserInventory();
    static QJsonArray validateBricklinkResponse(QObject* obj);

signals:
    void dataBaseUpdated();
    void dataBaseUpdatedWithOrders();

private slots:
    void parseJsonOrderItem(int orderID);

private:
    Q_DISABLE_COPY(BrickLink)
    void parseJsonColors();
    void parseJsonOrders();
    void parseJsonUserInventory();
};

#endif // BRICKLINK_H
