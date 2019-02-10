#ifndef COLORDELEGATE_H
#define COLORDELEGATE_H

#include <QModelIndex>
#include <QPixmap>
#include <QSize>
#include <QSqlRelationalDelegate>

class ColorDelegate : public QSqlRelationalDelegate
{
public:
    ColorDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

#endif // COLORDELEGATE_H
