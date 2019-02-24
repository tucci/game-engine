#ifndef DOCKSCENEHIERARCHY_H
#define DOCKSCENEHIERARCHY_H


#include <QMainWindow>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QHash>
#include <QMenu>

#include "dockinterface.h"
#include <customdockwidget.h>




class SceneHierarchyProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    SceneHierarchyProxyModel(QObject* parent = 0);
signals:
    void items_filtered(const QModelIndex& index) const;
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

};



class DockSceneHierarchy : public DockInterface {
    Q_OBJECT
public:
    DockSceneHierarchy(QWidget* parent);
    ~DockSceneHierarchy() override;
    QStandardItem* add_entity(uint64_t entity_id, bool is_root);
    void add_entity_child(uint64_t entity_id, uint64_t child_entity_id);
    void set_entity_name(uint64_t entity_id, const QString& name);
protected:
    bool eventFilter(QObject* o, QEvent* e) override;


private slots:
    void after_items_filtered(const QModelIndex& index);
    void context_menu_event(const QPoint& point);
    void entity_selection_changed(const QItemSelection& selected, const QItemSelection& deselected);


    void new_entity_from_editor();
    void copy_entitys();
    void paste_entitys();
    void duplicate_entitys();
    void rename_entity();
    void delete_entitys();
private:
    QList<uint64_t> entity_copy_list;
    QHash<uint64_t, QStandardItem*> entity_map;
    SceneHierarchyProxyModel* proxy_model;
    QItemSelectionModel* selection_model;
    QStandardItemModel* scene_tree_model;
    QTreeView* scene_tree_view;
    QLineEdit* scene_hierarchy_filter;


    // Context action
    QMenu* context_menu;
    QAction* new_entity_action;
    QAction* copy_entity_action;
    QAction* paste_entity_action;

    QAction* duplicate_entity_action;
    QAction* rename_entity_action;
    QAction* delete_entity_action;

    void create_context_menu();






};

#endif // DOCKSCENEHIERARCHY_H
