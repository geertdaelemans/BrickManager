#include "bricklink.h"
#include "sqldatabase.h"
#include "datamodel.h"

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

void BrickLink::parseJsonOrderItem(int orderID)
{
    DataModel *model = new DataModel(Tables::orderitem, QString::number(orderID));
    model->initiateSqlTable();
    QJsonArray batchArray = BrickLink::validateBricklinkResponse(sender());
    int batchNumber = 0;
    foreach(const QJsonValue &batch, batchArray) {
        batchNumber++;
        QJsonArray itemArray = batch.toArray();
        foreach(const QJsonValue &item, itemArray) {
            QJsonObject object = item.toObject();
            QMap<QString, QVariant> fields;
            fields["inventory_id"] = object.value("inventory_id").toVariant();
            fields["item_no"] = object.value("item").toObject().value("no").toVariant();
            fields["item_name"] = object.value("item").toObject().value("name").toVariant();
            fields["item_type"] = object.value("item").toObject().value("type").toVariant();
            fields["category_id"] = object.value("item").toObject().value("category_id").toVariant();
            fields["category_name"] = SqlDatabase::getCategoryById(fields["category_id"].toInt());
            fields["color_id"] = object.value("color_id").toVariant();
            fields["color_name"] = object.value("color_name").toVariant();
            fields["quantity"] = object.value("quantity").toVariant();
            fields["new_or_used"] = object.value("new_or_used").toVariant();
            fields["completeness"] = object.value("completeness").toVariant();
            fields["unit_price"] = object.value("unit_price").toVariant();
            fields["unit_price_final"] = object.value("unit_price_final").toVariant();
            fields["disp_unit_price"] = object.value("disp_unit_price").toVariant();
            fields["disp_unit_price_final"] = object.value("disp_unit_price_final").toVariant();
            fields["currency_code"] = object.value("currency_code").toVariant();
            fields["disp_currency_code"] = object.value("disp_currency_code").toVariant();
            fields["remarks"] = object.value("remarks").toVariant();
            fields["description"] = object.value("description").toVariant();
            fields["weight"] = object.value("weight").toVariant();
            fields["batchnumber"] = batchNumber;
            QSqlError error = model->addItemToTable(fields);
        }
    }
    emit dataBaseUpdated();
}

void BrickLink::parseJsonOrders()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    DataModel *model = new DataModel(Tables::orders);
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QMap<QString, QVariant> fields;
            fields["order_id"] = object.value("order_id").toVariant();
            fields["date_ordered"] = object.value("date_ordered").toVariant();
            fields["seller_name"] = object.value("seller_name").toVariant();
            fields["store_name"] = object.value("store_name").toVariant();
            fields["buyer_name"] = object.value("buyer_name").toVariant();
            fields["total_count"] = object.value("total_count").toVariant();
            fields["unique_count"] = object.value("unique_count").toVariant();
            fields["status"] = object.value("status").toVariant();
            fields["payment_method"] = object.value("payment").toObject().value("method").toVariant();
            fields["payment_status"] = object.value("payment").toObject().value("status").toVariant();
            fields["payment_date_paid"] = object.value("payment").toObject().value("date_paid").toVariant();
            fields["payment_currency_code"] = object.value("payment").toObject().value("currency_code").toVariant();
            fields["cost_subtotal"] = object.value("cost").toObject().value("subtotal").toVariant();
            fields["cost_grand_total"] = object.value("cost").toObject().value("grand_total").toVariant();
            fields["cost_currency_code"] = object.value("cost").toObject().value("currency_code").toVariant();
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
    DataModel *model = new DataModel(Tables::userinventories);
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QMap<QString, QVariant> fields;
            fields["inventory_id"] = object.value("inventory_id").toVariant();
            fields["item_no"] = object.value("item").toObject().value("no").toVariant();
            fields["item_name"] = object.value("item").toObject().value("name").toVariant();
            fields["item_type"] = object.value("item").toObject().value("type").toVariant();
            fields["category_id"] = object.value("item").toObject().value("category_id").toVariant();
            fields["category_name"] = SqlDatabase::getCategoryById(fields["category_id"].toInt());
            fields["color_id"] = object.value("color_id").toVariant();
            fields["color_name"] = object.value("color_name").toVariant();
            fields["quantity"] = object.value("quantity").toVariant();
            fields["new_or_used"] = object.value("new_or_used").toVariant();
            fields["completeness"] = object.value("completeness").toVariant();
            fields["unit_price"] = object.value("unit_price").toVariant();
            fields["bind_id"] = object.value("bind_id").toVariant();
            fields["description"] = object.value("description").toVariant();
            fields["remarks"] = object.value("remarks").toVariant();
            fields["bulk"] = object.value("bulk").toVariant();
            fields["is_retain"] = object.value("is_retain").toVariant();
            fields["is_stock_room"] = object.value("is_stock_room").toVariant();
            fields["date_created"] = object.value("date_created").toVariant();
            fields["my_cost"] = object.value("my_cost").toVariant();
            fields["sale_rate"] = object.value("sale_rate").toVariant();
            fields["tier_quantity1"] = object.value("tier_quantity1").toVariant();
            fields["tier_quantity2"] = object.value("tier_quantity2").toVariant();
            fields["tier_quantity3"] = object.value("tier_quantity3").toVariant();
            fields["tier_price1"] = object.value("tier_price1").toVariant();
            fields["tier_price2"] = object.value("tier_price2").toVariant();
            fields["tier_price3"] = object.value("tier_price3").toVariant();
            fields["my_weight"] = object.value("my_weight").toVariant();
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
