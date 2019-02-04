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
    void importCategories();

private:
    BrickLink bricklink;
    void parseJson();
};

#endif // CATEGORY_H
