#include "colors.h"

#include <QtCore>

Colors::Colors() : QObject()
{
    if(colors.size() == 0)
    {
        importColors();
    };
}

Colors::~Colors()
{
}

void Colors::importColors()
{
    QUrl url("https://api.bricklink.com/api/store/v1/colors");
    QVariantMap parameters;
    QNetworkReply *reply = bricklink.get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &Colors::parseJson);
}

void Colors::parseJson()
{
    QJsonParseError parseError;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error()) {
        qCritical() << "Category::getColors ->"
                    << reply->errorString();
        return;
    }
    const auto data = reply->readAll();
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error) {
        qCritical() << "Colors::getColor. Error at:" << parseError.offset
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
                int index = object.value("color_id").toInt();
                QString name = object.value("color_name").toString();
                colors.insert(index, name);
            }
        }
        qInfo("Initialized %d colors.", colors.size());
    }
}

QString Colors::getColor(int index)
{
    return colors.value(index);
}
