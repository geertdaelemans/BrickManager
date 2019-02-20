#include "listmodeldelegate.h"
#include "sqldatabase.h"

#include <QtWidgets>

ListModelDelegate::ListModelDelegate(QObject *parent)
    : QSqlRelationalDelegate(parent)
{

}

void ListModelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ?
        (option.state & QStyle::State_Active) ?
                    QPalette::Normal :
                    QPalette::Inactive :
                    QPalette::Disabled;

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.color(cg, QPalette::Highlight));

    QString columnName = model->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
    if(columnName == tr("Color")) {
        QColor color;
        QRect colorBox = option.rect;
        colorBox.setWidth(colorBox.height());
        QRect textBox = option.rect;
        textBox.translate(option.rect.height() + 8, 8);
        textBox.setWidth(textBox.width() - option.rect.height() - 8);
        QString colorName = model->data(index, Qt::DisplayRole).toString();
        QString colorCode = "#" + SqlDatabase::getColorCodeByName(colorName);
        color.setNamedColor(colorCode);
        painter->fillRect(colorBox, color);
        painter->drawText(textBox, Qt::TextSingleLine, colorName);
    } else
        QSqlRelationalDelegate::paint(painter, option, index);
}

