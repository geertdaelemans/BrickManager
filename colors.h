#ifndef COLORS_H
#define COLORS_H

#include "bricklink.h"

#include <QtCore>

class Colors : public QObject
{
    Q_OBJECT

public:
    explicit Colors();
    ~Colors();
    QString getColor(int index);
    void importColors();

private:
    BrickLink bricklink;
    void parseJson();
    static QMap<int, QString> colors;
};

#endif // COLORS_H
