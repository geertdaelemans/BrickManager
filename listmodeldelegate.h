#ifndef COLORDELEGATE_H
#define COLORDELEGATE_H

#include <QModelIndex>
#include <QPixmap>
#include <QSize>
#include <QSqlRelationalDelegate>

class ListModelDelegate : public QSqlRelationalDelegate
{
public:
    ListModelDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

#endif // COLORDELEGATE_H
