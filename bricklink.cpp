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


void BrickLink::importColors()
{
    QUrl url("https://api.bricklink.com/api/store/v1/colors");
    QVariantMap parameters;
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &BrickLink::parseJsonColors);
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
