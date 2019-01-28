#ifndef CATEGORY_H
#define CATEGORY_H

#include "bricklink.h"

#include <QtCore>

class Category : public QObject
{
    Q_OBJECT

public:
    explicit Category();
    ~Category();
    QString getCategory(int index);
    void importCategories();

private:
    BrickLink bricklink;
    void parseJson();
    static QMap<int, QString> categories;
};

#endif // CATEGORY_H
