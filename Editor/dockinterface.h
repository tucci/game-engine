#ifndef DOCKINTERFACE_H
#define DOCKINTERFACE_H

#include <QMainWindow>
#include "customdockwidget.h"


class EditorMenubar;

class DockInterface: public QObject {
    Q_OBJECT
public:
    DockInterface(QWidget* parent);
    virtual ~DockInterface() = 0;
    virtual void set_visibility(bool visibile);
    CustomDockWidget* dock_widget;
protected:
    QWidget* parent_window;

    bool visibility;
};

#endif // DOCKINTERFACE_H
