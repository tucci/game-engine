#include "dockassets.h"

DockAssets::DockAssets(QWidget* parent) : DockInterface (parent) {
    dock_widget = parent_window->findChild<CustomDockWidget*>("DockAssets");
    QString root_path = "C:\\Users\\Steven\\Documents\\Visual Studio 2015\\Projects\\Engine\\x64\\Release\\Assets\\";
    file_model.setRootPath(root_path);
    //file_model.setRootPath(QDir::currentPath());




    file_tree_view = parent_window->findChild<QTreeView*>("assets_file_treeview");



    file_tree_view->setModel(&file_model);

    file_tree_view->setAnimated(false);
    file_tree_view->setIndentation(20);


    file_tree_view->setRootIndex(file_model.index(root_path));
    int col_count = file_model.columnCount();

    for (int i = 1 ; i < col_count; i++) {
        file_tree_view->hideColumn(i);
    }

}


DockAssets::~DockAssets() {

}


