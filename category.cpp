#include "category.h"

#include <QtCore>

Category::Category() : QObject()
{
    if(categories.size() == 0)
    {
        importCategories();
    };
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
    QJsonParseError parseError;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error()) {
        qCritical() << "Category::getCategory ->"
                    << reply->errorString();
        return;
    }
    const auto data = reply->readAll();
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error) {
        qCritical() << "Category::getCategory. Error at:" << parseError.offset
                    << parseError.errorString();
        return;
    } else {
        QString strReply = static_cast<QString>(data);
        QJsonObject jsonObject = document.object();
        const auto array = jsonObject["data"].toArray();
        if (array.size()) {
            for (const auto &value : array) {
                Q_ASSERT(value.isObject());
                const auto object = value.toObject();
                int index = object.value("category_id").toInt();
                QString name = object.value("category_name").toString();
                categories.insert(index, name);
            }
        }
        qInfo("Initialized %d categories.", categories.size());
    }
}

QString Category::getCategory(int index)
{
    return categories.value(index);
}
