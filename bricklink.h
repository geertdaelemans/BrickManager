#ifndef BRICKLINK_H
#define BRICKLINK_H

#include <QtCore>
#include <QtNetwork>
#include <QtNetworkAuth>

class BrickLink : public QOAuth1
{
    Q_OBJECT

public:
    BrickLink(QObject *parent = nullptr);

private:
    Q_DISABLE_COPY(BrickLink)
};

#endif // BRICKLINK_H
