#ifndef EDITORMENUBAR_H
#define EDITORMENUBAR_H

#include <QMenuBar>
#include <QWidget>
#include <QAction>

class MainWindow;

enum class MenubarDataType {
    NONE,
    DOCK_GAME_VISIBILITY_CHANGE,
    DOCK_SCENE_HIERARCHY_VISIBILITY_CHANGE,
    DOCK_ENTITY_DETAILS_VISIBILITY_CHANGE,
    DOCK_ASSETS_VISIBILITY_CHANGE,
    DOCK_CONSOLE_VISIBILITY_CHANGE,
};

struct MenubarData {
    MenubarDataType type;
    bool dock_visibility;
};

class EditorMenubar : public QObject {
    Q_OBJECT
public:
    EditorMenubar();
    void init_editor_menubar(QWidget* parent_window);


signals:
    void menubar_data_changed(const MenubarData&);

public slots:
    // Window actions
    // These are called when the toggle button is clicked in the menubar
    void on_action_game_toggle(bool checked);
    void on_action_scene_hierarchy_toggle(bool checked);
    void on_action_entity_details_toggle(bool checked);
    void on_action_assets_toggle(bool checked);
    void on_action_console_toggle(bool checked);

    // These are called when the dock is closed with the top right "x" button
    void on_scene_hierarchy_dock_closed();
    void on_entity_details_dock_closed();
    void on_assets_dock_closed();
    void on_console_dock_closed();

private:
    QWidget* parent_window;
    QMenuBar* menu_bar;
    QAction* show_game_window;
    QAction* show_scene_hierarchy_window;
    QAction* show_entity_details_window;
    QAction* show_assets_window;
    QAction* show_console_window;
private slots:
    // File actions
    void on_actionNew_Project_triggered();
    void on_actionOpen_Project_triggered();
    void on_actionBuild_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();

    // Scene Actions
    void on_actionCreate_Empty_Entity_triggered();
    void on_actionCreate_Camera_triggered();
    void on_actionCreate_Light_triggered();
    void on_actionPlane_triggered();
    void on_actionCube_triggered();
    void on_actionSphere_triggered();

    // Asset Actions
    void on_actionImport_triggered();
    void on_actionMaterial_triggered();
    void on_actionFolder_triggered();

};

#endif // EDITORMENUBAR_H
