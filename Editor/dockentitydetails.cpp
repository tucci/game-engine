#include "dockentitydetails.h"

DockEntityDetails::DockEntityDetails(QWidget* parent)  : DockInterface (parent) {
    dock_widget = parent_window->findChild<CustomDockWidget*>("DockEntityDetails");
}

DockEntityDetails::~DockEntityDetails()
{

}

