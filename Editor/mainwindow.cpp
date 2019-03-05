#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QWindow>
#include <QMessageBox>
#include <QCloseEvent>

//#include <qpa/qplatformnativeinterface.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);




    game_widget = this->findChild<QWidget*>("content");
    game_layout = this->findChild<QVBoxLayout*>("gameLayout");

    game_widget->installEventFilter(this);


    menubar.init_editor_menubar(this);
    dock_scene_hierarchy = new DockSceneHierarchy(this);
    dock_entity_details = new DockEntityDetails(this);
    dock_assets = new DockAssets(this);
    dock_console = new DockConsole(this);
    //dock_scene_hierarchy = DockSceneHierarchy(this);
    //dock_entity_details.init_dock(this);
    //dock_assets.init_dock(this);
    //dock_console.init_dock(this);

    // Setup Dock/Tab settings
    this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    QMainWindow::tabifyDockWidget(dock_console->dock_widget, dock_assets->dock_widget);
    QMainWindow::setDockOptions(QMainWindow::dockOptions() | QMainWindow::GroupedDragging);
    QMainWindow::setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);


    QObject::connect(dock_scene_hierarchy->dock_widget, &CustomDockWidget::dockClosed, &menubar, &EditorMenubar::on_scene_hierarchy_dock_closed);
    QObject::connect(dock_entity_details->dock_widget, &CustomDockWidget::dockClosed, &menubar, &EditorMenubar::on_entity_details_dock_closed);
    QObject::connect(dock_assets->dock_widget, &CustomDockWidget::dockClosed, &menubar, &EditorMenubar::on_assets_dock_closed);
    QObject::connect(dock_console->dock_widget, &CustomDockWidget::dockClosed, &menubar, &EditorMenubar::on_console_dock_closed);


    connect(&menubar, &EditorMenubar::menubar_data_changed, this, &MainWindow::handle_menubar_change);


    QObject* parent_process = this;


    // Release build
    QString program = "C:/Users/Steven/Documents/Visual Studio 2015/Projects/Engine2/Engine/x64/Release/Engine.exe";
    QString working_dir = "C:/Users/Steven/Documents/Visual Studio 2015/Projects/Engine2/Engine/x64/Release/";

    // Debug build
    //QString program = "\"C:/Users/Steven/Documents/Visual Studio 2015/Projects/Engine/x64/Debug/Engine.exe\"";
    //QString working_dir = "\"C:/Users/Steven/Documents/Visual Studio 2015/Projects/Engine/x64/Debug/Engine/\"";

    QStringList args  = {};

    engine_process = new QProcess(parent_process);

    //connect (engine_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()));
    //connect (engine_process, SIGNAL(readyReadStandardError()), this, SLOT(processOutput()));


    // NOTE: we need to set the working directory because the engine expects the working directory to be in the engine folder
    // When we create the process in qt, the working directory is set to the parent process. Which is this current qt process
    // TODO: the working directory and program directory use different paths.
    // When it comes time to release version of the editor, the working directory should be the same as the program directory

    connect (engine_process, SIGNAL(started()), this, SLOT(process_started()));


    engine_process->setWorkingDirectory(working_dir);
    engine_process->start(program, args, QIODevice::ReadWrite);
    //engine_process->startDetached(program, args, working_dir);
    setFocus();
    //engine_interface.init_engine_interface(this, game_widget, game_layout, dock_console, dock_scene_hierarchy);

}

MainWindow::~MainWindow() {
    delete dock_scene_hierarchy;
    delete dock_entity_details;
    delete dock_assets;
    delete dock_console;

    delete ui;

}


void MainWindow::process_started() {
    qDebug() << "process started";
    engine_interface.init_engine_interface(this, game_widget, game_layout,
                                           dock_console,
                                           dock_scene_hierarchy);
}

void MainWindow::processOutput() {
    qInfo(engine_process->readAllStandardOutput());  // read normal output
    qInfo(engine_process->readAllStandardError()); // read error channel
}

void MainWindow::closeEvent (QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Editor",
                                                                   tr("Are you sure you want to close the editor?\n"),
                                                                   QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::Yes);

       if (resBtn != QMessageBox::Yes) {
           event->ignore();
       } else {
           clean_up_editor_resources_after_close();
           event->accept();
       }
}

void MainWindow::handle_menubar_change(const MenubarData& data) {
    switch (data.type) {
        case MenubarDataType::NONE: {
            break;
        }
        case MenubarDataType::DOCK_GAME_VISIBILITY_CHANGE: {
            //game_dock.setVisibility(data.dock_visibility);
            break;
        }
        case MenubarDataType::DOCK_SCENE_HIERARCHY_VISIBILITY_CHANGE: {
            dock_scene_hierarchy->set_visibility(data.dock_visibility);
            break;
        }
        case MenubarDataType::DOCK_CONSOLE_VISIBILITY_CHANGE: {
            dock_console->set_visibility(data.dock_visibility);
            break;
        }
        case MenubarDataType::DOCK_ENTITY_DETAILS_VISIBILITY_CHANGE: {
            dock_entity_details->set_visibility(data.dock_visibility);
            break;
        }
        case MenubarDataType::DOCK_ASSETS_VISIBILITY_CHANGE: {
            dock_assets->set_visibility(data.dock_visibility);
            break;
        }
    }
}

void MainWindow::clean_up_editor_resources_after_close() {
	engine_interface.destroy_engine_interface();
}


