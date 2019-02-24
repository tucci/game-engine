#include "dockinterface.h"
#include <QDebug>

DockInterface::DockInterface(QWidget* parent){
    this->parent_window = parent;
}

DockInterface::~DockInterface(){}


void DockInterface::set_visibility(bool visibile) {
    this->visibility = visibile;
    dock_widget->setHidden(!visibile);
}

