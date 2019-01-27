#include "bricklink.h"

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

BrickLink::BrickLink(QObject *parent) :
    QOAuth1(parent)
{
    QSettings settings;
    setSignatureMethod(QOAuth1::SignatureMethod(0));        // HMAC_SHA1
    setAuthorizationUrl(QUrl("https://api.bricklink.com/api/store/v1"));
    setClientCredentials(settings.value("credentials/consumerKey").toString(), settings.value("credentials/consumerSecret").toString());
    setTokenCredentials(settings.value("credentials/tokenValue").toString(), settings.value("credentials/tokenSecret").toString());
}

