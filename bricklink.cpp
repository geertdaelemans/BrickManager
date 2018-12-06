#include "bricklink.h"

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

BrickLink::BrickLink(QObject *parent) :
    QOAuth1(parent)
{
    setSignatureMethod(QOAuth1::SignatureMethod(0));        // HMAC_SHA1
    setAuthorizationUrl(QUrl("https://api.bricklink.com/api/store/v1/orders"));
    setClientCredentials("<ConsumerKey>", "<ConsumerSecret>");
    setTokenCredentials("<TokenValue>", "<TokenSecret>");
}
