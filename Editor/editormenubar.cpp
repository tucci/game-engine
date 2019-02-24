#include "editormenubar.h"

#include <QDebug>
#include <QFileDialog>

EditorMenubar::EditorMenubar()
{

}


void EditorMenubar::init_editor_menubar(QWidget* parent_window) {
    this->parent_window = parent_window;
    menu_bar = parent_window->findChild<QMenuBar*>("menuBar");

    // Get menu bar and child items
    show_game_window = parent_window->findChild<QAction*>("actionGame", Qt::FindChildrenRecursively);
    show_scene_hierarchy_window = parent_window->findChild<QAction*>("actionScene_Hierarchy", Qt::FindChildrenRecursively);
    show_entity_details_window = parent_window->findChild<QAction*>("actionEntity_Details", Qt::FindChildrenRecursively);
    show_assets_window = parent_window->findChild<QAction*>("actionAssets", Qt::FindChildrenRecursively);
    show_console_window = parent_window->findChild<QAction*>("actionConsole", Qt::FindChildrenRecursively);



    // File actions
    connect(parent_window->findChild<QAction*>("actionNew_Project", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionNew_Project_triggered);
    connect(parent_window->findChild<QAction*>("actionOpen_Project", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionOpen_Project_triggered);
    connect(parent_window->findChild<QAction*>("actionBuild", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionBuild_triggered);
    connect(parent_window->findChild<QAction*>("actionSave", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionSave_triggered);
    connect(parent_window->findChild<QAction*>("actionExit", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionExit_triggered);


    connect(parent_window->findChild<QAction*>("actionCreate_Empty_Entity", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionCreate_Empty_Entity_triggered);
    connect(parent_window->findChild<QAction*>("actionCreate_Camera", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionCreate_Camera_triggered);
    connect(parent_window->findChild<QAction*>("actionCreate_Light", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionCreate_Light_triggered);

    connect(parent_window->findChild<QAction*>("actionPlane", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionPlane_triggered);
    connect(parent_window->findChild<QAction*>("actionCube", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionCube_triggered);
    connect(parent_window->findChild<QAction*>("actionSphere", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionSphere_triggered);

    connect(parent_window->findChild<QAction*>("actionImport", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionImport_triggered);
    connect(parent_window->findChild<QAction*>("actionMaterial", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionMaterial_triggered);
    connect(parent_window->findChild<QAction*>("actionFolder", Qt::FindChildrenRecursively), &QAction::triggered, this, &EditorMenubar::on_actionFolder_triggered);




    connect(show_game_window, &QAction::triggered, this, &EditorMenubar::on_action_game_toggle);
    connect(show_scene_hierarchy_window, &QAction::triggered, this, &EditorMenubar::on_action_scene_hierarchy_toggle);
    connect(show_entity_details_window, &QAction::triggered, this, &EditorMenubar::on_action_entity_details_toggle);
    connect(show_assets_window, &QAction::triggered, this, &EditorMenubar::on_action_assets_toggle);
    connect(show_console_window, &QAction::triggered, this, &EditorMenubar::on_action_console_toggle);


    show_game_window->setChecked(true);
    show_scene_hierarchy_window->setChecked(true);
    show_entity_details_window->setChecked(true);
    show_assets_window->setChecked(true);
    show_console_window->setChecked(true);
}


// File actions
void EditorMenubar::on_actionNew_Project_triggered() {
    qDebug() << "new project";
}

void EditorMenubar::on_actionOpen_Project_triggered() {

    // NOTE: this will open the dialog and will block until something is selected
    auto file = QFileDialog::getOpenFileName(parent_window, tr("Open Project"), QString(), "*.epj");
    if (file.isNull() || file.isEmpty()) {
        // do nothing
    } else {
        qDebug() << "open project file" << file;
    }




}
void EditorMenubar::on_actionBuild_triggered() {
    qDebug() << "build project";
}


void EditorMenubar::on_actionSave_triggered() {
    qDebug() << "save project";
}

void EditorMenubar::on_actionExit_triggered() {
    qDebug() << "exit editor";
    parent_window->close();
}

// Scene Actions
void EditorMenubar::on_actionCreate_Empty_Entity_triggered() {
    qDebug() << "new entity";
}

void EditorMenubar::on_actionCreate_Camera_triggered() {
    qDebug() << "new camera";
}

void EditorMenubar::on_actionCreate_Light_triggered() {
    qDebug() << "new light";
}

void EditorMenubar::on_actionPlane_triggered() {
    qDebug() << "new plane";
}

void EditorMenubar::on_actionCube_triggered() {
    qDebug() << "new cube";
}

void EditorMenubar::on_actionSphere_triggered() {
    qDebug() << "new sphere";
}

// Asset Actions
void EditorMenubar::on_actionImport_triggered() {
    qDebug() << "import";
}

void EditorMenubar::on_actionMaterial_triggered() {
    qDebug() << "new material";
}

void EditorMenubar::on_actionFolder_triggered() {
    qDebug() << "new folder";
}

// Window actions
void EditorMenubar::on_action_game_toggle(bool checked) {
    MenubarData data;
    data.type = MenubarDataType::DOCK_GAME_VISIBILITY_CHANGE;
    data.dock_visibility = checked;
    menubar_data_changed(data);
}

void EditorMenubar::on_action_scene_hierarchy_toggle(bool checked) {
    MenubarData data;
    data.type = MenubarDataType::DOCK_SCENE_HIERARCHY_VISIBILITY_CHANGE;
    data.dock_visibility = checked;
    menubar_data_changed(data);
}

void EditorMenubar::on_action_entity_details_toggle(bool checked) {
    MenubarData data;
    data.type = MenubarDataType::DOCK_ENTITY_DETAILS_VISIBILITY_CHANGE;
    data.dock_visibility = checked;
    menubar_data_changed(data);
}

void EditorMenubar::on_action_assets_toggle(bool checked) {
    MenubarData data;
    data.type = MenubarDataType::DOCK_ASSETS_VISIBILITY_CHANGE;
    data.dock_visibility = checked;
    menubar_data_changed(data);
}

void EditorMenubar::on_action_console_toggle(bool checked) {
    MenubarData data;
    data.type = MenubarDataType::DOCK_CONSOLE_VISIBILITY_CHANGE;
    data.dock_visibility = checked;
    menubar_data_changed(data);
}




void EditorMenubar::on_scene_hierarchy_dock_closed() {
    show_scene_hierarchy_window->setChecked(false);
}
void EditorMenubar::on_entity_details_dock_closed() {
    show_entity_details_window->setChecked(false);
}
void EditorMenubar::on_assets_dock_closed() {
    show_assets_window->setChecked(false);
}

void EditorMenubar::on_console_dock_closed() {
    show_console_window->setChecked(false);
}



