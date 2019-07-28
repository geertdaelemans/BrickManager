#ifndef CONTAINER_H
#define CONTAINER_H

#include <QtCore>

class Container
{

public:
    Container(QString name, QString item_id)
        : m_name(name), m_item_id(item_id) {}
    void setName(const QString name) { m_name = name; }
    void setItemID(const QString item_id) { m_item_id = item_id; }
    const QString getName() const { return m_name; }
    const QString getItemID() const { return m_item_id; }

private:
    QString m_name;
    QString m_item_id;
};

#endif // CONTAINER_H
