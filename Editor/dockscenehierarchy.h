#ifndef DOCKSCENEHIERARCHY_H
#define DOCKSCENEHIERARCHY_H


#include <QMainWindow>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QHash>
#include <QMenu>
#include <QMimeData>

#include "dockinterface.h"
#include "customdockwidget.h"





class SceneHierarchyProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    SceneHierarchyProxyModel(QObject* parent = nullptr);
signals:
    void items_filtered(const QModelIndex& index) const;
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};


class EntityItemModel : public QStandardItemModel {

public:
    explicit EntityItemModel(QObject *parent = nullptr);
    bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList &indexes) const override;
};


class DockSceneHierarchy : public DockInterface {
    Q_OBJECT
public:
    DockSceneHierarchy(QWidget* parent);
    ~DockSceneHierarchy() override;




    void add_entity(QHash<uint64_t, QStandardItem*>& startup_build_entity_map, uint64_t entity_id, bool is_root, const QString& name);
    void add_entity_child(QHash<uint64_t, QStandardItem*>& startup_build_entity_map, uint64_t entity_id, uint64_t child_entity_id);

protected:
    bool eventFilter(QObject* o, QEvent* e) override;

signals:
    void notify_entity_new();
    void notify_entity_delete(const QList<uint64_t>& entitys);
    void notify_entity_selection_change(const QSet<uint64_t>& entities);
    void notify_entity_duplicate(const QList<uint64_t>& entities);

public slots:


    void respond_new_entity(uint64_t entity_id, const QString& name);
    void respond_delete_entitys(const QList<uint64_t>& entities);
    void respond_duplicate_entitys(const QList<uint64_t>& entities);


private slots:
    void context_menu_event(const QPoint& point);
    void entity_selection_changed(const QItemSelection& selected, const QItemSelection& deselected);


    void request_new_entity();

    void copy_entitys();
    void paste_entitys();
    void request_duplicate_entitys();
    void rename_entity();
    void request_delete_entitys();
private:
    QSet<uint64_t> entity_set;
    QSet<uint64_t> selected_entities;
    QList<uint64_t> entity_copy_list;

    SceneHierarchyProxyModel* proxy_model;
    QItemSelectionModel* selection_model;
    EntityItemModel* scene_tree_model;
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
    void recursive_respond_delete(uint64_t entity_id_to_delete, QModelIndex parent = QModelIndex());

};

#endif // DOCKSCENEHIERARCHY_H
