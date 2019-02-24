/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <checklist.h>
#include <customdockwidget.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Project;
    QAction *actionOpen_Project;
    QAction *actionSave;
    QAction *actionExit;
    QAction *actionCreate_Empty_Entity;
    QAction *actionCreate_Camera;
    QAction *actionCreate_Light;
    QAction *actionPlane;
    QAction *actionCube;
    QAction *actionSphere;
    QAction *actionImport;
    QAction *actionFolder;
    QAction *actionMaterial;
    QAction *actionGame;
    QAction *actionScene_Hierarchy;
    QAction *actionEntity_Details;
    QAction *actionAssets;
    QAction *actionConsole;
    QAction *actionBuild;
    QWidget *content;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *gameLayout;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuScene;
    QMenu *menuCreate_Primative;
    QMenu *menuAssets;
    QMenu *menuCreate;
    QMenu *menuWindow;
    CustomDockWidget *DockConsole;
    QWidget *dockWidgetContents_9;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *console_info_checkbox;
    QCheckBox *console_warning_checkbox;
    QCheckBox *console_error_checkbox;
    QSpacerItem *horizontalSpacer_3;
    CheckList *console_column_select;
    QSpacerItem *horizontalSpacer_4;
    CheckList *console_tag_select;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *console_search;
    QSpacerItem *horizontalSpacer;
    QPushButton *console_clear_console;
    QTableView *console_tableview;
    CustomDockWidget *DockScene;
    QWidget *dockWidgetContents_10;
    QVBoxLayout *verticalLayout;
    QLineEdit *scene_hierarchy_filter;
    QTreeView *scene_hierarchy_treeview;
    CustomDockWidget *DockEntityDetails;
    QWidget *dockWidgetContents_11;
    QHBoxLayout *horizontalLayout_8;
    QListView *listView;
    CustomDockWidget *DockAssets;
    QWidget *dockWidgetContents_13;
    QHBoxLayout *horizontalLayout;
    QTreeView *assets_file_treeview;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(947, 587);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        actionNew_Project = new QAction(MainWindow);
        actionNew_Project->setObjectName(QString::fromUtf8("actionNew_Project"));
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionCreate_Empty_Entity = new QAction(MainWindow);
        actionCreate_Empty_Entity->setObjectName(QString::fromUtf8("actionCreate_Empty_Entity"));
        actionCreate_Camera = new QAction(MainWindow);
        actionCreate_Camera->setObjectName(QString::fromUtf8("actionCreate_Camera"));
        actionCreate_Light = new QAction(MainWindow);
        actionCreate_Light->setObjectName(QString::fromUtf8("actionCreate_Light"));
        actionPlane = new QAction(MainWindow);
        actionPlane->setObjectName(QString::fromUtf8("actionPlane"));
        actionCube = new QAction(MainWindow);
        actionCube->setObjectName(QString::fromUtf8("actionCube"));
        actionSphere = new QAction(MainWindow);
        actionSphere->setObjectName(QString::fromUtf8("actionSphere"));
        actionImport = new QAction(MainWindow);
        actionImport->setObjectName(QString::fromUtf8("actionImport"));
        actionFolder = new QAction(MainWindow);
        actionFolder->setObjectName(QString::fromUtf8("actionFolder"));
        actionMaterial = new QAction(MainWindow);
        actionMaterial->setObjectName(QString::fromUtf8("actionMaterial"));
        actionGame = new QAction(MainWindow);
        actionGame->setObjectName(QString::fromUtf8("actionGame"));
        actionGame->setCheckable(true);
        actionScene_Hierarchy = new QAction(MainWindow);
        actionScene_Hierarchy->setObjectName(QString::fromUtf8("actionScene_Hierarchy"));
        actionScene_Hierarchy->setCheckable(true);
        actionEntity_Details = new QAction(MainWindow);
        actionEntity_Details->setObjectName(QString::fromUtf8("actionEntity_Details"));
        actionEntity_Details->setCheckable(true);
        actionAssets = new QAction(MainWindow);
        actionAssets->setObjectName(QString::fromUtf8("actionAssets"));
        actionAssets->setCheckable(true);
        actionConsole = new QAction(MainWindow);
        actionConsole->setObjectName(QString::fromUtf8("actionConsole"));
        actionConsole->setCheckable(true);
        actionBuild = new QAction(MainWindow);
        actionBuild->setObjectName(QString::fromUtf8("actionBuild"));
        content = new QWidget(MainWindow);
        content->setObjectName(QString::fromUtf8("content"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(content->sizePolicy().hasHeightForWidth());
        content->setSizePolicy(sizePolicy);
        content->setFocusPolicy(Qt::StrongFocus);
        content->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_5 = new QVBoxLayout(content);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        gameLayout = new QVBoxLayout();
        gameLayout->setSpacing(6);
        gameLayout->setObjectName(QString::fromUtf8("gameLayout"));

        verticalLayout_5->addLayout(gameLayout);

        MainWindow->setCentralWidget(content);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 947, 20));
        menuBar->setStyleSheet(QString::fromUtf8(""));
        menuBar->setDefaultUp(false);
        menuBar->setNativeMenuBar(false);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuFile->setTearOffEnabled(false);
        menuScene = new QMenu(menuBar);
        menuScene->setObjectName(QString::fromUtf8("menuScene"));
        menuCreate_Primative = new QMenu(menuScene);
        menuCreate_Primative->setObjectName(QString::fromUtf8("menuCreate_Primative"));
        menuAssets = new QMenu(menuBar);
        menuAssets->setObjectName(QString::fromUtf8("menuAssets"));
        menuCreate = new QMenu(menuAssets);
        menuCreate->setObjectName(QString::fromUtf8("menuCreate"));
        menuWindow = new QMenu(menuBar);
        menuWindow->setObjectName(QString::fromUtf8("menuWindow"));
        MainWindow->setMenuBar(menuBar);
        DockConsole = new CustomDockWidget(MainWindow);
        DockConsole->setObjectName(QString::fromUtf8("DockConsole"));
        DockConsole->setStyleSheet(QString::fromUtf8(""));
        dockWidgetContents_9 = new QWidget();
        dockWidgetContents_9->setObjectName(QString::fromUtf8("dockWidgetContents_9"));
        dockWidgetContents_9->setLayoutDirection(Qt::LeftToRight);
        verticalLayout_3 = new QVBoxLayout(dockWidgetContents_9);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        console_info_checkbox = new QCheckBox(dockWidgetContents_9);
        console_info_checkbox->setObjectName(QString::fromUtf8("console_info_checkbox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(console_info_checkbox->sizePolicy().hasHeightForWidth());
        console_info_checkbox->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(console_info_checkbox);

        console_warning_checkbox = new QCheckBox(dockWidgetContents_9);
        console_warning_checkbox->setObjectName(QString::fromUtf8("console_warning_checkbox"));
        sizePolicy1.setHeightForWidth(console_warning_checkbox->sizePolicy().hasHeightForWidth());
        console_warning_checkbox->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(console_warning_checkbox);

        console_error_checkbox = new QCheckBox(dockWidgetContents_9);
        console_error_checkbox->setObjectName(QString::fromUtf8("console_error_checkbox"));
        sizePolicy1.setHeightForWidth(console_error_checkbox->sizePolicy().hasHeightForWidth());
        console_error_checkbox->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(console_error_checkbox);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        console_column_select = new CheckList(dockWidgetContents_9);
        console_column_select->setObjectName(QString::fromUtf8("console_column_select"));
        console_column_select->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
        console_column_select->setMinimumContentsLength(0);

        horizontalLayout_2->addWidget(console_column_select);

        horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        console_tag_select = new CheckList(dockWidgetContents_9);
        console_tag_select->setObjectName(QString::fromUtf8("console_tag_select"));

        horizontalLayout_2->addWidget(console_tag_select);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        console_search = new QLineEdit(dockWidgetContents_9);
        console_search->setObjectName(QString::fromUtf8("console_search"));
        console_search->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(console_search->sizePolicy().hasHeightForWidth());
        console_search->setSizePolicy(sizePolicy2);
        console_search->setFocusPolicy(Qt::StrongFocus);
        console_search->setMaxLength(1600);
        console_search->setClearButtonEnabled(false);

        horizontalLayout_2->addWidget(console_search);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        console_clear_console = new QPushButton(dockWidgetContents_9);
        console_clear_console->setObjectName(QString::fromUtf8("console_clear_console"));
        sizePolicy1.setHeightForWidth(console_clear_console->sizePolicy().hasHeightForWidth());
        console_clear_console->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(console_clear_console);


        verticalLayout_3->addLayout(horizontalLayout_2);

        console_tableview = new QTableView(dockWidgetContents_9);
        console_tableview->setObjectName(QString::fromUtf8("console_tableview"));
        console_tableview->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        console_tableview->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_3->addWidget(console_tableview);

        DockConsole->setWidget(dockWidgetContents_9);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(8), DockConsole);
        DockScene = new CustomDockWidget(MainWindow);
        DockScene->setObjectName(QString::fromUtf8("DockScene"));
        dockWidgetContents_10 = new QWidget();
        dockWidgetContents_10->setObjectName(QString::fromUtf8("dockWidgetContents_10"));
        verticalLayout = new QVBoxLayout(dockWidgetContents_10);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        scene_hierarchy_filter = new QLineEdit(dockWidgetContents_10);
        scene_hierarchy_filter->setObjectName(QString::fromUtf8("scene_hierarchy_filter"));

        verticalLayout->addWidget(scene_hierarchy_filter);

        scene_hierarchy_treeview = new QTreeView(dockWidgetContents_10);
        scene_hierarchy_treeview->setObjectName(QString::fromUtf8("scene_hierarchy_treeview"));
        scene_hierarchy_treeview->setHeaderHidden(true);

        verticalLayout->addWidget(scene_hierarchy_treeview);

        DockScene->setWidget(dockWidgetContents_10);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), DockScene);
        DockEntityDetails = new CustomDockWidget(MainWindow);
        DockEntityDetails->setObjectName(QString::fromUtf8("DockEntityDetails"));
        dockWidgetContents_11 = new QWidget();
        dockWidgetContents_11->setObjectName(QString::fromUtf8("dockWidgetContents_11"));
        horizontalLayout_8 = new QHBoxLayout(dockWidgetContents_11);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        listView = new QListView(dockWidgetContents_11);
        listView->setObjectName(QString::fromUtf8("listView"));

        horizontalLayout_8->addWidget(listView);

        DockEntityDetails->setWidget(dockWidgetContents_11);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), DockEntityDetails);
        DockAssets = new CustomDockWidget(MainWindow);
        DockAssets->setObjectName(QString::fromUtf8("DockAssets"));
        DockAssets->setStyleSheet(QString::fromUtf8(""));
        dockWidgetContents_13 = new QWidget();
        dockWidgetContents_13->setObjectName(QString::fromUtf8("dockWidgetContents_13"));
        horizontalLayout = new QHBoxLayout(dockWidgetContents_13);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        assets_file_treeview = new QTreeView(dockWidgetContents_13);
        assets_file_treeview->setObjectName(QString::fromUtf8("assets_file_treeview"));
        assets_file_treeview->header()->setVisible(false);

        horizontalLayout->addWidget(assets_file_treeview);

        DockAssets->setWidget(dockWidgetContents_13);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), DockAssets);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuScene->menuAction());
        menuBar->addAction(menuAssets->menuAction());
        menuBar->addAction(menuWindow->menuAction());
        menuFile->addAction(actionNew_Project);
        menuFile->addAction(actionOpen_Project);
        menuFile->addSeparator();
        menuFile->addAction(actionBuild);
        menuFile->addAction(actionSave);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuScene->addAction(actionCreate_Empty_Entity);
        menuScene->addSeparator();
        menuScene->addAction(menuCreate_Primative->menuAction());
        menuScene->addAction(actionCreate_Camera);
        menuScene->addAction(actionCreate_Light);
        menuCreate_Primative->addAction(actionPlane);
        menuCreate_Primative->addAction(actionCube);
        menuCreate_Primative->addAction(actionSphere);
        menuAssets->addAction(menuCreate->menuAction());
        menuAssets->addSeparator();
        menuAssets->addAction(actionImport);
        menuCreate->addAction(actionFolder);
        menuCreate->addSeparator();
        menuCreate->addAction(actionMaterial);
        menuWindow->addAction(actionGame);
        menuWindow->addAction(actionScene_Hierarchy);
        menuWindow->addAction(actionEntity_Details);
        menuWindow->addAction(actionAssets);
        menuWindow->addAction(actionConsole);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionNew_Project->setText(QApplication::translate("MainWindow", "New Project", nullptr));
#ifndef QT_NO_SHORTCUT
        actionNew_Project->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_NO_SHORTCUT
        actionOpen_Project->setText(QApplication::translate("MainWindow", "Open Project", nullptr));
#ifndef QT_NO_SHORTCUT
        actionOpen_Project->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        actionSave->setText(QApplication::translate("MainWindow", "Save", nullptr));
#ifndef QT_NO_SHORTCUT
        actionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_NO_SHORTCUT
        actionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
        actionCreate_Empty_Entity->setText(QApplication::translate("MainWindow", "Create Empty Entity", nullptr));
        actionCreate_Camera->setText(QApplication::translate("MainWindow", "Create Camera", nullptr));
        actionCreate_Light->setText(QApplication::translate("MainWindow", "Create Light", nullptr));
        actionPlane->setText(QApplication::translate("MainWindow", "Plane", nullptr));
        actionCube->setText(QApplication::translate("MainWindow", "Cube", nullptr));
        actionSphere->setText(QApplication::translate("MainWindow", "Sphere", nullptr));
        actionImport->setText(QApplication::translate("MainWindow", "Import", nullptr));
        actionFolder->setText(QApplication::translate("MainWindow", "Folder", nullptr));
        actionMaterial->setText(QApplication::translate("MainWindow", "Material", nullptr));
        actionGame->setText(QApplication::translate("MainWindow", "Game", nullptr));
        actionScene_Hierarchy->setText(QApplication::translate("MainWindow", "Scene Hierarchy", nullptr));
        actionEntity_Details->setText(QApplication::translate("MainWindow", "Entity Details", nullptr));
        actionAssets->setText(QApplication::translate("MainWindow", "Assets", nullptr));
        actionConsole->setText(QApplication::translate("MainWindow", "Console", nullptr));
        actionBuild->setText(QApplication::translate("MainWindow", "Build", nullptr));
#ifndef QT_NO_SHORTCUT
        actionBuild->setShortcut(QApplication::translate("MainWindow", "Ctrl+B", nullptr));
#endif // QT_NO_SHORTCUT
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuScene->setTitle(QApplication::translate("MainWindow", "Scene", nullptr));
        menuCreate_Primative->setTitle(QApplication::translate("MainWindow", "Create Primative", nullptr));
        menuAssets->setTitle(QApplication::translate("MainWindow", "Assets", nullptr));
        menuCreate->setTitle(QApplication::translate("MainWindow", "Create", nullptr));
        menuWindow->setTitle(QApplication::translate("MainWindow", "Window", nullptr));
        DockConsole->setWindowTitle(QApplication::translate("MainWindow", "Console", nullptr));
        console_info_checkbox->setText(QApplication::translate("MainWindow", "Info", nullptr));
        console_warning_checkbox->setText(QApplication::translate("MainWindow", "Warning", nullptr));
        console_error_checkbox->setText(QApplication::translate("MainWindow", "Error", nullptr));
        console_column_select->setCurrentText(QString());
#ifndef QT_NO_TOOLTIP
        console_search->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        console_search->setText(QString());
        console_search->setPlaceholderText(QApplication::translate("MainWindow", "Search/Filter Logs", nullptr));
        console_clear_console->setText(QApplication::translate("MainWindow", "Clear Logs", nullptr));
        DockScene->setWindowTitle(QApplication::translate("MainWindow", "Scene", nullptr));
        scene_hierarchy_filter->setPlaceholderText(QApplication::translate("MainWindow", "Search Entitys", nullptr));
        DockEntityDetails->setWindowTitle(QApplication::translate("MainWindow", "Entity Details", nullptr));
        DockAssets->setWindowTitle(QApplication::translate("MainWindow", "Assets", nullptr));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
