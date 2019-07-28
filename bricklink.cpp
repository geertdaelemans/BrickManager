#include "bricklink.h"
#include "sqldatabase.h"
#include "datamodel.h"

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

    m_pictures.transfer = new CTransfer();
    m_pictures.update_iv = 0;

    connect(m_pictures.transfer, SIGNAL(finished(CTransfer::Job*)), this, SLOT(pictureJobFinished(CTransfer::Job*)));
}


BrickLink* BrickLink::s_inst = nullptr;
const QString ItemType::m_table_prefix = "cat_";
const QString ItemType::m_names[8] = {"Book", "Catalog", "Gear", "Instruction", "Minifig", "Original box", "Set", "Part"};
const char ItemType::m_picture_ids[8] = {'B', 'C', 'G', 'I', 'M', 'O', 'S', 'P'};
const QString ItemType::m_api_names[8] = {"book", "catalog", "gear", "instruction", "minifig", "original_box", "set", "part"};
const Tables ItemType::m_table_names[8] = {Tables::books, Tables::catalogs, Tables::gear, Tables::instructions, Tables::minifigs, Tables::boxes, Tables::sets, Tables::parts};

BrickLink* BrickLink::inst()
{
    if (!s_inst) {
        s_inst = new BrickLink();
    }
    return s_inst;
}

ItemType::ItemType(const QString name)
{
    if (name == "Book") {
        m_api_name = "book";
        m_table_name = Tables::books;
        m_picture_id = 'B';
    } else if (name == "Catalog") {
        m_api_name = "catalog";
        m_table_name = Tables::catalogs;
        m_picture_id = 'C';
    } else if (name == "Gear") {
        m_api_name = "gear";
        m_table_name = Tables::gear;
        m_picture_id = 'G';
    } else if (name == "Instruction") {
        m_api_name = "instruction";
        m_table_name = Tables::instructions;
        m_picture_id = 'I';
    } else if (name == "Minifig") {
        m_api_name = "minifig";
        m_table_name = Tables::minifigs;
        m_picture_id = 'M';
    } else if (name == "Original box") {
        m_api_name = "original_box";
        m_table_name = Tables::boxes;
        m_picture_id = 'O';
    } else if (name == "Set") {
        m_api_name = "set";
        m_table_name = Tables::sets;
        m_picture_id = 'S';
    } else {
        m_api_name = "part";
        m_table_name = Tables::parts;
        m_picture_id = 'P';
    }
    m_id = 1;
}

ItemType::~ItemType()
{

}

QString ItemType::getSqlName(QString name)
{
    return m_table_prefix + m_api_names->at(m_names->indexOf(name));
}


BrickLink::Item::Item()
{

}

BrickLink::Item::~Item()
{

}

const BrickLink::Item* BrickLink::item(const QString itemType, const char *id) const
{
    Item* key = new Item();
    key->m_item_type = new ItemType(itemType);
    key->m_id = const_cast <char *>(id);
    return key;
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

void BrickLink::importOrderItem(int orderID)
{
    QUrl url;
    url = "https://api.bricklink.com/api/store/v1/orders/" + QString::number(orderID) + "/items";
    QVariantMap parameters;
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, [orderID, this](){this->parseJsonOrderItem(orderID);});

}

void BrickLink::importOrders(bool filed)
{
    QUrl url("https://api.bricklink.com/api/store/v1/orders");
    QVariantMap parameters;
    parameters.insert("direction", "in");
    parameters.insert("filed", (filed ? "true" : "false"));
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &BrickLink::parseJsonOrders);
}

void BrickLink::importUserInventory()
{
    QUrl url("https://api.bricklink.com/api/store/v1/inventories");
    QVariantMap parameters;
    QNetworkReply *reply = this->get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &BrickLink::parseJsonUserInventory);
}

QString BrickLink::getItemInformation(QString type, QString no)
{
    QUrl url(QString("https://api.bricklink.com/api/store/v1/items/%1/%2").arg(type).arg(no));
    QNetworkReply *reply = this->get(url);

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonArray array = BrickLink::validateBricklinkResponse(reply);
        foreach(const QJsonValue &batch, array) {
            return batch.toString();
        }
    }
    return QString();
}

void BrickLink::parseJsonOrderItem(int orderID)
{
    DataModel *model = new DataModel(Tables::orderitem, QString::number(orderID));
    model->initiateSqlTable();
    QJsonArray batchArray = BrickLink::validateBricklinkResponse(sender());
    int batchNumber = 0;
    foreach(const QJsonValue &batch, batchArray) {
        batchNumber++;
        QJsonArray itemArray = batch.toArray();
        foreach(const QJsonValue &item, itemArray) {
            QJsonObject object = item.toObject();
            QMap<QString, QVariant> fields;
            fields["inventory_id"] = object.value("inventory_id").toVariant();
            fields["item_no"] = object.value("item").toObject().value("no").toVariant();
            fields["item_name"] = object.value("item").toObject().value("name").toVariant();
            fields["item_type"] = object.value("item").toObject().value("type").toVariant();
            fields["category_id"] = object.value("item").toObject().value("category_id").toVariant();
            fields["category_name"] = SqlDatabase::getCategoryById(fields["category_id"].toInt());
            fields["color_id"] = object.value("color_id").toVariant();
            fields["color_name"] = object.value("color_name").toVariant();
            fields["quantity"] = object.value("quantity").toVariant();
            fields["new_or_used"] = object.value("new_or_used").toVariant();
            fields["completeness"] = object.value("completeness").toVariant();
            fields["unit_price"] = object.value("unit_price").toVariant();
            fields["unit_price_final"] = object.value("unit_price_final").toVariant();
            fields["disp_unit_price"] = object.value("disp_unit_price").toVariant();
            fields["disp_unit_price_final"] = object.value("disp_unit_price_final").toVariant();
            fields["currency_code"] = object.value("currency_code").toVariant();
            fields["disp_currency_code"] = object.value("disp_currency_code").toVariant();
            fields["remarks"] = object.value("remarks").toVariant();
            fields["description"] = object.value("description").toVariant();
            fields["weight"] = object.value("weight").toVariant();
            fields["batchnumber"] = batchNumber;
            QSqlError error = model->addItemToTable(fields);
        }
    }
    emit dataBaseUpdated();
}

void BrickLink::parseJsonOrders()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    DataModel *model = new DataModel(Tables::orders);
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QMap<QString, QVariant> fields;
            fields["order_id"] = object.value("order_id").toVariant();
            fields["date_ordered"] = object.value("date_ordered").toVariant();
            fields["seller_name"] = object.value("seller_name").toVariant();
            fields["store_name"] = object.value("store_name").toVariant();
            fields["buyer_name"] = object.value("buyer_name").toVariant();
            fields["total_count"] = object.value("total_count").toVariant();
            fields["unique_count"] = object.value("unique_count").toVariant();
            fields["status"] = object.value("status").toVariant();
            fields["payment_method"] = object.value("payment").toObject().value("method").toVariant();
            fields["payment_status"] = object.value("payment").toObject().value("status").toVariant();
            fields["payment_date_paid"] = object.value("payment").toObject().value("date_paid").toVariant();
            fields["payment_currency_code"] = object.value("payment").toObject().value("currency_code").toVariant();
            fields["cost_subtotal"] = object.value("cost").toObject().value("subtotal").toVariant();
            fields["cost_grand_total"] = object.value("cost").toObject().value("grand_total").toVariant();
            fields["cost_currency_code"] = object.value("cost").toObject().value("currency_code").toVariant();
            QSqlError error = model->addItemToTable(fields);
            if(error.type() != QSqlError::NoError)
                qDebug() << "parseJsonOrders()" << error.text();
        }
    }
    emit dataBaseUpdatedWithOrders();
}

void BrickLink::parseJsonUserInventory()
{
    QJsonArray array = BrickLink::validateBricklinkResponse(sender());
    DataModel *model = new DataModel(Tables::userinventories);
    if (array.size()) {
        for (auto value : array) {
            Q_ASSERT(value.isObject());
            const auto object = value.toObject();
            QMap<QString, QVariant> fields;
            fields["id"] = object.value("inventory_id").toVariant();
            fields["item_no"] = object.value("item").toObject().value("no").toVariant();
            fields["item_name"] = object.value("item").toObject().value("name").toVariant();
            fields["item_type"] = object.value("item").toObject().value("type").toVariant();
            fields["category_id"] = object.value("item").toObject().value("category_id").toVariant();
            fields["category_name"] = SqlDatabase::getCategoryById(fields["category_id"].toInt());
            fields["color_id"] = object.value("color_id").toVariant();
            fields["color_name"] = object.value("color_name").toVariant();
            fields["quantity"] = object.value("quantity").toVariant();
            fields["new_or_used"] = object.value("new_or_used").toVariant();
            fields["completeness"] = object.value("completeness").toVariant();
            fields["unit_price"] = object.value("unit_price").toVariant();
            fields["bind_id"] = object.value("bind_id").toVariant();
            fields["description"] = object.value("description").toVariant();
            fields["remarks"] = object.value("remarks").toVariant();
            fields["bulk"] = object.value("bulk").toVariant();
            fields["is_retain"] = object.value("is_retain").toVariant();
            fields["is_stock_room"] = object.value("is_stock_room").toVariant();
            fields["date_created"] = object.value("date_created").toVariant();
            fields["my_cost"] = object.value("my_cost").toVariant();
            fields["sale_rate"] = object.value("sale_rate").toVariant();
            fields["tier_quantity1"] = object.value("tier_quantity1").toVariant();
            fields["tier_quantity2"] = object.value("tier_quantity2").toVariant();
            fields["tier_quantity3"] = object.value("tier_quantity3").toVariant();
            fields["tier_price1"] = object.value("tier_price1").toVariant();
            fields["tier_price2"] = object.value("tier_price2").toVariant();
            fields["tier_price3"] = object.value("tier_price3").toVariant();
            fields["my_weight"] = object.value("my_weight").toVariant();
            QSqlError error = model->addItemToTable(fields);
            if(error.type() != QSqlError::NoError)
                qDebug() << "parseJsonUserInventory()" << error.text();
        }
    }
    emit dataBaseUpdated();
}


/**
 * Checks the reply comming from the HTTP GET request and returns the data.
 * @param incomming reply following the HTTP GET request.
 * @return JSON array containing the requested data.
 */
QJsonArray BrickLink::validateBricklinkResponse(QObject* obj) {
    auto reply = qobject_cast<QNetworkReply*>(obj);
    Q_ASSERT(reply);
    QJsonParseError parseError;
    QJsonArray output;
    const auto data = reply->readAll();
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error) {
        qCritical() << "BrickLink connection error while parsing JSON. Error at:" << parseError.offset
                    << parseError.errorString();
    } else {
        QString strReply = static_cast<QString>(data);
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        if (jsonObject.value("data").isArray())
            output = jsonObject.value("data").toArray();
        else {
            QJsonObject tester = jsonObject.value("data").toObject();
            output.push_front(tester.value("image_url"));
        }
    }
    return output;
}


BrickLink::Picture *BrickLink::picture(const Item *item, const BrickLink::Color *color, bool high_priority)
{
    if (!item) {
        return nullptr;
    }

    QString key;
    if (color) {
        key = QString("%1@%2@%3").arg(item->itemType()->pictureId()).arg(item->id()).arg(color->id());
    } else{
        key = QString("%1@%2").arg(item->itemType()->pictureId()).arg(item->id());
    }

    qDebug() << "Key" << key;
    qDebug() << "High priority" << high_priority;

    Picture *pic = m_pictures.cache[key];
    bool need_to_load = false;

    if (!pic) {
        pic = new Picture(item, color, key);

        if (color) {
            m_pictures.cache.insert(key, pic);
        }

        need_to_load = true;
    }

    qDebug() << "Need to load" << need_to_load;

    if (high_priority) {
        if (!pic->valid()) {
//            pic->load_from_disk();
        }

//        if (updateNeeded(pic->valid(), pic->lastUpdate(), m_pictures.update_iv)) {
//            addPictureToUpdate(pic, high_priority);
//        }
    } else if (need_to_load) {
        pic->addRef();
        m_pictures.diskload.append(pic);

        if (m_pictures.diskload.count() == 1) {
            QTimer::singleShot(0, BrickLink::inst(), SLOT(pictureIdleLoader()));
        }
    }

    return pic;
}




BrickLink::Picture *BrickLink::largePicture(const Item *item, bool high_priority)
{
    if (!item)
    {
        return nullptr;
    }
    return picture(item, nullptr, high_priority);
}

BrickLink::Picture::Picture(const Item *item, const Color *color, const QString key)
{
    m_item = item;
    m_color = color;

    m_valid = false;
//    m_update_status = Ok;
    m_key = key.isNull() ? nullptr : key.toLatin1().data();
}

BrickLink::Picture::~Picture()
{
    if (m_key)
    {
//        QPixmapCache::remove(m_key);
        free(m_key);
    }
}

//void BrickLink::Picture::load_from_disk()
//{
//    QString path;
//    bool large = !m_color;

//    if (!large && m_item->itemType()->hasColors()) {
//        path = BrickLink::inst()->dataPath(m_item, m_color);
//    } else {
//        path = BrickLink::inst()->dataPath(m_item);
//    }

//    if (path.isEmpty()) {
//        return;
//    }

//    path += large ? "large.png" : "small.png";

//    QFileInfo fi(path);
//    if (fi.exists()) {
//        if (fi.size() > 0)
//        {
//            m_valid = m_image.load(path);
//        }
//        else
//        {
//            if (!large && m_item && m_item->itemType())
//            {
//                m_image = BrickLink::inst()->noImage(m_item->itemType()->imageSize())->convertToImage();
//            }
//            else
//            {
//                m_image.create(0, 0, 32);
//            }

//            m_valid = true;
//        }

//        m_fetched = fi.lastModified();
//    }
//    else
//    {
//        m_valid = false;
//    }

//    QPixmapCache::remove(m_key);
//}

void BrickLink::pictureJobFinished(CTransfer::Job *j)
{
    if (!j || !j->data() || !j->userObject())
    {
        return;
    }

    qDebug() << "pictureJobFinished";

//    if (!j->contentType().isEmpty())
//    {
//        // Image names fetched, now fetch actual images

//        bool ok = false;
//        QMap<QString, QJsonObject> pictureItems;
//        if (!j->failed())
//        {
//            QJsonParseError jerror;
//            QJsonDocument jsonResponse = QJsonDocument::fromJson(*(j->data()), &jerror);
//            ok = jerror.error == QJsonParseError::NoError && jsonResponse.object()["meta"].toObject()["code"].toInt() == 200;

//            if (ok)
//            {
//                QJsonArray pgData = jsonResponse.object()["data"].toArray();
//                foreach (QJsonValue value, pgData)
//                {
//                    QJsonObject obj = value.toObject();
//                    QJsonObject item = obj["item"].toObject();

//                    QString key;
//                    QTextStream (&key) << "" << item["type"].toString() << "@" << item["no"].toString() << "@" << obj["color_id"].toInt();

//                    pictureItems.insert(key.toLower(), obj, true);
//                }
//            }
//        }

//        QVector<BrickLink::Picture *> *pictures = static_cast<QVector<BrickLink::Picture *> *>(j->userObject());

//        foreach (Picture *pic, *pictures)
//        {
//            if (ok)
//            {
//                QString key;
//                QTextStream (&key) << "" << pic->item()->itemType()->apiName() << "@" << pic->item()->id() << "@" << pic->color()->id();

//                QJsonObject pictureItem = pictureItems[key.toLower()];
//                if (pictureItem["image_url"].isString())
//                {
//                    QString imageUrl = pictureItem["image_url"].toString();

//                    if (imageUrl == "N/A")
//                    {
//                        bool large = !pic->color();
//                        QString path;

//                        if (!large && pic->item()->itemType()->hasColors())
//                        {
//                            path = BrickLink::inst()->dataPath(pic->item(), pic->color());
//                        }
//                        else
//                        {
//                            path = BrickLink::inst()->dataPath(pic->item());
//                        }

//                        path.append(large ? "large.png" : "small.png");
//                        pic-> m_update_status = Ok;

//                        QFile f(path);
//                        f.open(QIODevice::WriteOnly | QIODevice::Truncate);
//                        f.close();

//                        pic->load_from_disk();

//                        return;
//                    }

//                    m_pictures.transfer->get("http:" + imageUrl, CKeyValueList(), 0, pic);
//                }
//            }
//        }

//        return;
//    }

//    Picture *pic = static_cast<Picture *>(j->userObject());
//    bool large = !pic->color();

//    pic->m_update_status = UpdateFailed;

//    if (!j->failed())
//    {
//        QString path;
//        QImage img;

//        if (!large && pic->item()->itemType()->hasColors())
//        {
//            path = BrickLink::inst()->dataPath(pic->item(), pic->color());
//        }
//        else
//        {
//            path = BrickLink::inst()->dataPath(pic->item());
//        }

//        if (!path.isEmpty())
//        {
//            path.append(large ? "large.png" : "small.png");
//            pic-> m_update_status = Ok;

//            if (j->effectiveUrl().toLower().find("noimage", 0) == -1 && j->data()->size() && img.loadFromData(*j->data()))
//            {
//                if (!large)
//                {
//                    img = img.convertToFormat(QImage::Format_ARGB32, Qt::ColorOnly | Qt::DiffuseDither | Qt::DiffuseAlphaDither);
//                }

//                img.save(path, "PNG");
//            }
//            else
//            {
//                QFile f(path);
//                f.open(QIODevice::WriteOnly | QIODevice::Truncate);
//                f.close();

//                qWarning("No image !");
//            }

//            pic->load_from_disk();
//        }
//        else
//        {
//            qWarning("Couldn't get path to save image");
//        }
//    }
//    else if (large && j->responseCode() == 404 && j->url().right(3) == "jpg")
//    {
//        // no large JPG image -> try a GIF image instead
//        pic->m_update_status = Updating;

//        QString url = j->url();
//        url.replace(url.length() - 3, 3, "gif");

//        m_pictures.transfer->get(url, CKeyValueList(), 0, pic);
//        return;
//    }
//    else
//    {
//        qWarning("Image download failed");
//    }

//    emit pictureUpdated(pic);
//    pic->release();
}


