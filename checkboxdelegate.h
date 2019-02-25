#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QModelIndex>


namespace DELEGATE
{

    class CheckBoxDelegate
        : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        CheckBoxDelegate( QObject *parent );
        ~CheckBoxDelegate();

        void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
        bool editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index );
    };

}

#endif // CHECKBOXDELEGATE_H
