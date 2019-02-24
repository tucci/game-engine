#ifndef DOCKENTITYDETAILS_H
#define DOCKENTITYDETAILS_H



#include <QMainWindow>

#include "dockinterface.h"
#include <customdockwidget.h>

class DockEntityDetails : public DockInterface {
    Q_OBJECT
public:
    DockEntityDetails(QWidget* parent);
    ~DockEntityDetails();


};

#endif // DOCKENTITYDETAILS_H
