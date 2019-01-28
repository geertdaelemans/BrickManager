#include "bricklink.h"

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
