#include "customdockwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

CustomDockWidget::CustomDockWidget(QWidget *parent) : QDockWidget(parent) {
    //this->setParent(parent);
    connect(this, &CustomDockWidget::topLevelChanged, this, &CustomDockWidget::floating_changed);
}

void CustomDockWidget::closeEvent(QCloseEvent *event) {
    dockClosed();
}

void CustomDockWidget::floating_changed(bool floating)
{
    if (isFloating()) {
        setWindowFlags(Qt::CustomizeWindowHint |
            Qt::Window |
            Qt::WindowMinimizeButtonHint |
            Qt::WindowMaximizeButtonHint |
            Qt::WindowCloseButtonHint);
        show();
    }

}




//void CustomDockWidget::paintEvent(QPaintEvent *pe) {
//    QDockWidget::paintEvent(pe);
//    QStyleOption o;
//    o.initFrom(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
//}
