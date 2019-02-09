#include "bricklink.h"
#include "sqldatabase.h"

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
            int index = object.value("category_id").toInt();
            QString name = object.value("category_name").toString();
            int parent_id = object.value("parent_id").toInt();
            SqlDatabase::addCategory(index, name, parent_id);
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
            int index = object.value("color_id").toInt();
            QString name = object.value("color_name").toString();
            QString code = object.value("color_code").toString();
            QString type = object.value("color_type").toString();
            SqlDatabase::addColor(index, name, code, type);
        }
    }
}

void BrickLink::parseJsonUserInventory()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            int inventory_id = object.value("inventory_id").toInt();
            QString item_no = object.value("item").toObject().value("no").toString();
            QString item_name = object.value("item").toObject().value("name").toString();
            QString item_type = object.value("item").toObject().value("type").toString();
            int item_category_id = object.value("item").toObject().value("category_id").toInt();
            int color_id = object.value("color_id").toInt();
            QString color_name = object.value("color_name").toString();
            int quantity = object.value("quantity").toInt();
            QString new_or_used = object.value("new_or_used").toString();
            QString completeness = object.value("completeness").toString();
            double unit_price = object.value("unit_price").toVariant().toDouble();
            int bind_id = object.value("bind_id").toInt();
            QString description = object.value("description").toString();
            QString remarks = object.value("remarks").toString();
            int bulk = object.value("bulk").toInt();
            bool is_retain = object.value("is_retain").toBool();
            bool is_stock_room = object.value("is_stock_room").toBool();
            QDateTime date_created = QDateTime::fromString(object.value("date_created").toString(), "yyyy-MM-dd'T'HH:mm:ss.zzz'Z'");
            double my_cost = object.value("my_cost").toVariant().toDouble();
            int sale_rate = object.value("sale_rate").toInt();
            int tier_quantity1 = object.value("tier_quantity1").toInt();
            int tier_quantity2 = object.value("tier_quantity2").toInt();
            int tier_quantity3 = object.value("tier_quantity3").toInt();
            double tier_price1 = object.value("tier_price1").toVariant().toDouble();
            double tier_price2 = object.value("tier_price2").toVariant().toDouble();
            double tier_price3 = object.value("tier_price3").toVariant().toDouble();
            double my_weight = object.value("my_weight").toVariant().toDouble();
            QSqlError error = SqlDatabase::addUserInventory(inventory_id, item_no, item_name, item_type, item_category_id,
                                          color_id, color_name, quantity, new_or_used, completeness,
                                          unit_price, bind_id, description, remarks, bulk, is_retain,
                                          is_stock_room, date_created, my_cost, sale_rate, tier_quantity1,
                                          tier_quantity2, tier_quantity3, tier_price1, tier_price2, tier_price3,
                                          my_weight);
            if(error.type() != QSqlError::NoError)
                qDebug() << error.text();
        }
    }
    emit messageSent();
};


/*
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
