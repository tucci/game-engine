#include "consolelogitemdelegate.h"
#include <QtDebug>
#include <QPainter>

#include "logitem.h"

ConsoleLogItemDelegate::ConsoleLogItemDelegate(QObject* parent) : QStyledItemDelegate (parent) {
}


//QWidget* ConsoleLogItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const  {
//    qDebug() << "create editor";
//    QLabel* editor = new QLabel(parent);
//
//
//    //why isnt the text showing????
//
//    return editor;
//
//}
//
//void ConsoleLogItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
//    QStyledItemDelegate::setEditorData(editor, index);
//    qDebug() << "set editor data";
//    //auto item = index.model()->data(index).value<LogItem>();
//    QLabel* label = static_cast<QLabel*>(editor);
//
//    label->setText("test");
//}
//
//void ConsoleLogItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
//    qDebug() << "set model data";
//}
//
//void ConsoleLogItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
//    qDebug() << "update editor geometry";
//    editor->setGeometry(option.rect);
//}


void ConsoleLogItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    LogItem item = qvariant_cast<LogItem>(index.data());
    QString text = item.msg_str;
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    //ok it all works, now need a way to double click item to jump to log location
    //
    //also need to be able to highlight text to copy

    opt.text = "";
    //QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    QStyle *style = opt.widget->style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QRect rect = opt.rect;
    QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active)) {
        cg = QPalette::Inactive;
    }


    // set pen color
    if (opt.state & QStyle::State_Selected) {
        painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
    } else {
        painter->setPen(opt.palette.color(cg, QPalette::Text));
    }

    // draw 2 lines of text
    painter->drawText(QRect(rect.left(), rect.top(), rect.width(), rect.height()), opt.displayAlignment, text);


}

QSize ConsoleLogItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    return result;
}

