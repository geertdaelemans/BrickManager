#include "category.h"
#include "sqldatabase.h"

#include <QtCore>

Category::Category() : QObject()
{
    importCategories();
}

Category::~Category()
{
}

void Category::importCategories()
{
    QUrl url("https://api.bricklink.com/api/store/v1/categories");
    QVariantMap parameters;
    QNetworkReply *reply = bricklink.get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &Category::parseJson);
}

void Category::parseJson()
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
