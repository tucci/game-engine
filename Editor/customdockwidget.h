#ifndef CUSTOMDOCKWIDGET_H
#define CUSTOMDOCKWIDGET_H


#include <QDockWidget>

class CustomDockWidget : public QDockWidget
{
    Q_OBJECT
public:
     explicit CustomDockWidget(QWidget *parent = nullptr);


signals:
    void dockClosed();
private slots:
    void floating_changed(bool);



protected:
    void closeEvent(QCloseEvent *event);
};


#endif // CUSTOMDOCKWIDGET_H
