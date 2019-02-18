#include "bricklink.h"
#include "sqldatabase.h"
#include "datamodels.h"

#include <QtCore>
#include <QtNetwork>
#include <QMessageBox>

BrickLink::BrickLink(QObject *parent) :
    QOAuth1(parent)
{
    QSettings settings;
    setSignatureMethod(QOAuth1::SignatureMethod(QOAuth1::SignatureMethod::Hmac_Sha1));
    setAuthorizationUrl(QUrl("https://api.bricklink.com/api/store/v1"));
    setClientCredentials(
                settings.value("credentials/consumerKey").toString(),
                settings.value("credentials/consumerSecret").toString());
    setTokenCredentials(
                settings.value("credentials/tokenValue").toString(),
                settings.value("credentials/tokenSecret").toString());
}


bool BrickLink::checkConnection(QObject *parent)
{
    QUrl url("https://api.bricklink.com/api/store/v1/categories/1");
    QNetworkReply *reply = this->get(url);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    const auto data = reply->readAll();
    if (reply->error()) {
        QMessageBox::critical(qobject_cast<QWidget *> (parent), QObject::tr("Error"), reply->errorString());
        return false;
    }
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(data, &parseError);
    QJsonObject jsonObject = document.object();
    const auto array = jsonObject["meta"].toObject()["description"].toString();
    if (array != "OK") {
        QMessageBox::critical(qobject_cast<QWidget *> (parent), QObject::tr("Error"), array);
        return false;
    }
    return true;
}


void BrickLink::importCategories()
{
    QUrl url("https://api.bricklink.com/api/store/v1/categories");
    QVariantMap parameters;
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &BrickLink::parseJsonCategories);
}

void BrickLink::importColors()
{
    QUrl url("https://api.bricklink.com/api/store/v1/colors");
    QVariantMap parameters;
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &BrickLink::parseJsonColors);
}

void BrickLink::importOrderItem(int orderID)
{
    QUrl url;
    url = "https://api.bricklink.com/api/store/v1/orders/" + QString::number(orderID) + "/items";
    QVariantMap parameters;
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, [orderID, this](){this->parseJsonOrderItem(orderID);});

}

void BrickLink::importOrders(bool filed)
{
    QUrl url("https://api.bricklink.com/api/store/v1/orders");
    QVariantMap parameters;
    parameters.insert("direction", "in");
    parameters.insert("filed", (filed ? "true" : "false"));
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &BrickLink::parseJsonOrders);
}

void BrickLink::importUserInventory()
{
    QUrl url("https://api.bricklink.com/api/store/v1/inventories");
    QVariantMap parameters;
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &BrickLink::parseJsonUserInventory);
}

void BrickLink::parseJsonCategories()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QList<QVariant> fields;
            fields.append(object.value("category_id").toInt());
            fields.append(object.value("category_name").toString());
            fields.append(object.value("parent_id").toInt());
            TableModel *model = new TableModel(Tables::categories);
            model->addItemToTable(fields);
        }
    }
}

void BrickLink::parseJsonColors()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QList<QVariant> fields;
            fields.append(object.value("color_id").toInt());
            fields.append(object.value("color_name").toString());
            fields.append(object.value("color_code").toString());
            fields.append(object.value("color_type").toString());
            TableModel *model = new TableModel(Tables::colors);
            model->addItemToTable(fields);
        }
    }
}

void BrickLink::parseJsonOrderItem(int orderID)
{
    TableModel *model = new TableModel(Tables::orderitem, orderID);
    model->initiateSqlTable();
    //SqlDatabase::initiateOrderItemTable(orderID);
    QJsonArray batchArray = BrickLink::validateBricklinkResponse(sender());
    int batchNumber = 0;
    foreach(const QJsonValue &batch, batchArray) {
        batchNumber++;
        QJsonArray itemArray = batch.toArray();
        foreach(const QJsonValue &item, itemArray) {
            QJsonObject object = item.toObject();
            QList<QVariant> fields;
            fields.append(object.value("inventory_id").toInt());
            fields.append(object.value("item").toObject().value("no").toString());
            fields.append(object.value("item").toObject().value("name").toString());
            fields.append(object.value("item").toObject().value("type").toString());
            fields.append(object.value("item").toObject().value("category_id").toInt());
            fields.append(object.value("color_id").toInt());
            fields.append(object.value("quantity").toInt());
            fields.append(object.value("new_or_used").toString());
            fields.append(object.value("completeness").toString());
            fields.append(object.value("unit_price").toVariant().toDouble());
            fields.append(object.value("unit_price_final").toVariant().toDouble());
            fields.append(object.value("disp_unit_price").toVariant().toDouble());
            fields.append(object.value("disp_unit_price_final").toVariant().toDouble());
            fields.append(object.value("currency_code").toString());
            fields.append(object.value("disp_currency_code").toString());
            fields.append(object.value("remarks").toString());
            fields.append(object.value("description").toString());
            fields.append(object.value("weight").toVariant().toDouble());
            fields.append(batchNumber);
            QSqlError error = model->addItemToTable(fields);
        }
    }
    emit dataBaseUpdated();
}

void BrickLink::parseJsonOrders()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    TableModel *model = new TableModel(Tables::orders);
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QList<QVariant> fields;
            fields.append(object.value("order_id").toInt());
            fields.append(QDateTime::fromString(object.value("date_ordered").toString(), "yyyy-MM-dd'T'HH:mm:ss.zzz'Z'"));
            fields.append(object.value("seller_name").toString());
            fields.append(object.value("store_name").toString());
            fields.append(object.value("buyer_name").toString());
            fields.append(object.value("total_count").toInt());
            fields.append(object.value("unique_count").toInt());
            fields.append(object.value("status").toString());
            fields.append(object.value("payment").toObject().value("method").toString());
            fields.append(object.value("payment").toObject().value("status").toString());
            fields.append(QDateTime::fromString(object.value("payment").toObject().value("date_paid").toString(), "yyyy-MM-dd'T'HH:mm:ss.zzz'Z'"));
            fields.append(object.value("payment").toObject().value("currency_code").toString());
            fields.append(object.value("cost").toObject().value("subtotal").toVariant().toDouble());
            fields.append(object.value("cost").toObject().value("grand_total").toVariant().toDouble());
            fields.append(object.value("cost").toObject().value("currency_code").toString());
            QSqlError error = model->addItemToTable(fields);
            if(error.type() != QSqlError::NoError)
                qDebug() << error.text();
        }
    }
    emit dataBaseUpdatedWithOrders();
}

void BrickLink::parseJsonUserInventory()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QList<QVariant> fields;
            fields.append(object.value("inventory_id").toInt());
            fields.append(object.value("item").toObject().value("no").toString());
            fields.append(object.value("item").toObject().value("name").toString());
            fields.append(object.value("item").toObject().value("type").toString());
            fields.append(object.value("item").toObject().value("category_id").toInt());
            fields.append(object.value("color_id").toInt());
            fields.append(object.value("quantity").toInt());
            fields.append(object.value("new_or_used").toString());
            fields.append(object.value("completeness").toString());
            fields.append(object.value("unit_price").toVariant().toDouble());
            fields.append(object.value("bind_id").toInt());
            fields.append(object.value("description").toString());
            fields.append(object.value("remarks").toString());
            fields.append(object.value("bulk").toInt());
            fields.append(object.value("is_retain").toBool());
            fields.append(object.value("is_stock_room").toBool());
            fields.append(QDateTime::fromString(object.value("date_created").toString(), "yyyy-MM-dd'T'HH:mm:ss.zzz'Z'"));
            fields.append(object.value("my_cost").toVariant().toDouble());
            fields.append(object.value("sale_rate").toInt());
            fields.append(object.value("tier_quantity1").toInt());
            fields.append(object.value("tier_quantity2").toInt());
            fields.append(object.value("tier_quantity3").toInt());
            fields.append(object.value("tier_price1").toVariant().toDouble());
            fields.append(object.value("tier_price2").toVariant().toDouble());
            fields.append(object.value("tier_price3").toVariant().toDouble());
            fields.append(object.value("my_weight").toVariant().toDouble());
            TableModel *model = new TableModel(Tables::userinventories);
            QSqlError error = model->addItemToTable(fields);
            if(error.type() != QSqlError::NoError)
                qDebug() << error.text();
        }
    }
    emit dataBaseUpdated();
}


/**
 * Checks the reply comming from the HTTP GET request and returns the data.
 * @param incomming reply following the HTTP GET request.
 * @return JSON array containing the requested data.
 */
QJsonArray BrickLink::validateBricklinkResponse(QObject* obj) {
    auto reply = qobject_cast<QNetworkReply*>(obj);
    Q_ASSERT(reply);
    QJsonParseError parseError;
    QJsonArray output;
    const auto data = reply->readAll();
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error) {
        qCritical() << "BrickLink connection error while parsing JSON. Error at:" << parseError.offset
                    << parseError.errorString();
    } else {
        QString strReply = static_cast<QString>(data);
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        output = jsonObject.value("data").toArray();
    }
    return output;
}
