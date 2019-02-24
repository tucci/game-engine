#ifndef DOCKASSETS_H
#define DOCKASSETS_H



#include <QMainWindow>

#include "dockinterface.h"
#include <customdockwidget.h>

#include <QFileSystemModel>
#include <QTreeView>

class DockAssets : public DockInterface {
    Q_OBJECT
public:
    DockAssets(QWidget* parent);
    ~DockAssets();
private:
    QFileSystemModel file_model;
    QTreeView* file_tree_view;

};

#endif // DOCKASSETS_H
