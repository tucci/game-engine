#ifndef CONSOLELOGITEMDELEGATE_H
#define CONSOLELOGITEMDELEGATE_H



#include <QStyledItemDelegate>
#include <QLabel>


class ConsoleLogItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ConsoleLogItemDelegate(QObject* parent = 0);

    //QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    //void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    //void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    //void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


};

#endif // CONSOLELOGITEMDELEGATE_H
