#ifndef BRICKLINK_H
#define BRICKLINK_H

#include <QtNetworkAuth>

class BrickLink : public QOAuth1
{
    Q_OBJECT

public:
    BrickLink(QObject *parent = nullptr);
    bool checkConnection(QObject *parent = nullptr);
    void importCategories();
    void importColors();
    void importUserInventory();
    static QJsonArray validateBricklinkResponse(QObject* obj);

private:
    Q_DISABLE_COPY(BrickLink)
    void parseJsonCategories();
    void parseJsonColors();
    void parseJsonUserInventory();
};

#endif // BRICKLINK_H
