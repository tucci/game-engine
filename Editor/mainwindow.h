#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVBoxLayout>

#include <QTreeView>

#include <QProcess>
#include <qsurface.h>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QtPlatformHeaders/qwglnativecontext.h>



#include "engineinterface.h"

#include <customdockwidget.h>

#include "editormenubar.h"
#include "dockassets.h"
#include "dockscenehierarchy.h"
#include "dockconsole.h"
#include "dockentitydetails.h"


namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    EngineInterface engine_interface;



    //QWindow* engine_window;
    //QWidget* engine_widget;
    QProcess* engine_process;

    //QOpenGLWidget* gl_widget;
    //QOpenGLContext* engine_glcontext;
    //QOpenGLContext* qt_glcontext;

    QWidget* game_widget;
    QVBoxLayout* game_layout;


    // Menu bar
    EditorMenubar menubar;
    // Docks

    DockSceneHierarchy* dock_scene_hierarchy;
    DockEntityDetails* dock_entity_details;
    DockAssets* dock_assets;
    DockConsole* dock_console;


private slots:
    void process_started();

    void processOutput();
    void closeEvent (QCloseEvent *event);
    void handle_menubar_change(const MenubarData& data);


private:
    Ui::MainWindow *ui;
    void clean_up_editor_resources_after_close();
};

#endif // MAINWINDOW_H
