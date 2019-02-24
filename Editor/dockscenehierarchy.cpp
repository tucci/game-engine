#include "dockscenehierarchy.h"
#include <QDebug>

#include <QEvent>
#include <QKeyEvent>


SceneHierarchyProxyModel::SceneHierarchyProxyModel(QObject* parent) : QSortFilterProxyModel (parent) {
    setDynamicSortFilter(true);
}



bool SceneHierarchyProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  {
    QModelIndex entity_index = sourceModel()->index(sourceRow, 0, sourceParent);
    QVariant entity_variant = sourceModel()->data(entity_index);



    if (entity_index.isValid()) {
        int child_count = sourceModel()->rowCount(entity_index);
        for (int i = 0; i < child_count; i++) {
            if (filterAcceptsRow(i, entity_index)) {
                auto filterindex = sourceModel()->index(i, 0, entity_index);
                emit items_filtered(filterindex);
                return true;
            }
        }
        QString entity_name = entity_variant.toString();
        QRegExp filter = filterRegExp();
        filter.setCaseSensitivity(Qt::CaseInsensitive);
        return entity_name.contains(filter);

    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

DockSceneHierarchy::DockSceneHierarchy(QWidget* parent) : DockInterface (parent) {
    dock_widget = parent_window->findChild<CustomDockWidget*>("DockScene");
    scene_tree_view = parent_window->findChild<QTreeView*>("scene_hierarchy_treeview");
    scene_hierarchy_filter = parent_window->findChild<QLineEdit*>("scene_hierarchy_filter");


    proxy_model = new SceneHierarchyProxyModel(this->parent_window);
    scene_tree_model = new QStandardItemModel();


    proxy_model->setSourceModel(scene_tree_model);

    //crashes when deleting filtered items

    scene_tree_view->setModel(proxy_model);
    scene_tree_view->setContextMenuPolicy(Qt::CustomContextMenu);
    scene_tree_view->setDragDropMode(QAbstractItemView::InternalMove);
    scene_tree_view->installEventFilter(this);
    scene_tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    create_context_menu();
    selection_model = scene_tree_view->selectionModel();


    connect(scene_tree_view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(context_menu_event(const QPoint &)));
    connect(selection_model , SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(entity_selection_changed(const QItemSelection&,const QItemSelection&)));
    connect(scene_hierarchy_filter, &QLineEdit::textChanged, proxy_model, &SceneHierarchyProxyModel::setFilterWildcard);
    connect(proxy_model, &SceneHierarchyProxyModel::items_filtered, this, &DockSceneHierarchy::after_items_filtered);





}


DockSceneHierarchy::~DockSceneHierarchy()
{
    delete proxy_model;
    delete context_menu;
    delete scene_tree_model;
}

bool DockSceneHierarchy::eventFilter(QObject* o, QEvent* e) {


    if (o == this->scene_tree_view) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent* key = static_cast<QKeyEvent*>(e);

            if (key->key() == Qt::Key_Delete) {
                delete_entitys();
            }
            else if (key->key() == Qt::Key_N && key->modifiers().testFlag(Qt::ControlModifier) && key->modifiers().testFlag(Qt::ShiftModifier)) {
                new_entity_from_editor();
            }
            else if (key->key() == Qt::Key_C && key->modifiers().testFlag(Qt::ControlModifier)) {
                copy_entitys();
            }
            else if (key->key() == Qt::Key_V && key->modifiers().testFlag(Qt::ControlModifier)) {
                paste_entitys();
            }
            else if (key->key() == Qt::Key_Z && key->modifiers().testFlag(Qt::ControlModifier)) {
                qDebug() << "undo tree";
            }
            else if (key->key() == Qt::Key_Y && key->modifiers().testFlag(Qt::ControlModifier)) {
                qDebug() << "redo tree";
            }
            else if (key->key() == Qt::Key_D && key->modifiers().testFlag(Qt::ControlModifier)) {
                duplicate_entitys();
            }
            else if (key->key() == Qt::Key_F && key->modifiers().testFlag(Qt::ControlModifier)) {
                scene_hierarchy_filter->setFocus();
            }
            else if (key->key() == Qt::Key_A && key->modifiers().testFlag(Qt::ControlModifier)) {
                // Select all
                // This is handled already by the qtreeview events
            }
        }
    }
    return QObject::eventFilter(o, e);
}

void DockSceneHierarchy::create_context_menu() {
    context_menu = new QMenu(tr("Context menu"), this->parent_window);

    new_entity_action       = new QAction(tr("Create Entity"), this);
    copy_entity_action      = new QAction(tr("Copy"), this);
    paste_entity_action     = new QAction(tr("Paste"), this);
    duplicate_entity_action = new QAction(tr("Duplicate"), this);
    rename_entity_action    = new QAction(tr("Rename"), this);
    delete_entity_action    = new QAction(tr("Delete"), this);


    context_menu->addAction(copy_entity_action);
    context_menu->addAction(paste_entity_action);
    context_menu->addSeparator();
    context_menu->addAction(duplicate_entity_action);
    context_menu->addAction(rename_entity_action);
    context_menu->addSeparator();
    context_menu->addAction(delete_entity_action);
    context_menu->addSeparator();
    context_menu->addAction(new_entity_action);


    // These start disabled because there is currently no items selected
    // this will change when items are selected
    duplicate_entity_action->setEnabled(false);
    rename_entity_action->setEnabled(false);
    delete_entity_action->setEnabled(false);
    copy_entity_action->setEnabled(false);
    paste_entity_action->setEnabled(false);


    connect(new_entity_action, SIGNAL(triggered()), this, SLOT(new_entity_from_editor()));
    connect(copy_entity_action, SIGNAL(triggered()), this, SLOT(copy_entitys()));
    connect(paste_entity_action, SIGNAL(triggered()), this, SLOT(paste_entitys()));
    connect(duplicate_entity_action, SIGNAL(triggered()), this, SLOT(duplicate_entitys()));
    connect(rename_entity_action, SIGNAL(triggered()), this, SLOT(rename_entity()));
    connect(delete_entity_action, SIGNAL(triggered()), this, SLOT(delete_entitys()));


}

void DockSceneHierarchy::after_items_filtered(const QModelIndex& index) {
    //scene_tree_view->setExpanded(index, true);
    //scene_tree_view->expandAll();
}

void DockSceneHierarchy::context_menu_event(const QPoint& point) {
    paste_entity_action->setEnabled(entity_copy_list.count() > 0);
    context_menu->exec(this->parent_window->mapToGlobal(point));
}

void DockSceneHierarchy::entity_selection_changed(const QItemSelection& selected, const QItemSelection& deselected) {
    auto selected_items = selected.indexes();
    auto deselected_items = deselected.indexes();

    int selected_count = selected_items.count();
    bool any_selected = selected_items.count() > 0;

    duplicate_entity_action->setEnabled(any_selected);
    rename_entity_action->setEnabled(selected_count == 1);
    delete_entity_action->setEnabled(any_selected);
    copy_entity_action->setEnabled(any_selected);

    //for (const QModelIndex& index : selected_items) {
    //    auto item = scene_tree_model->itemFromIndex(index);
    //}
    //
    //for (const QModelIndex& index : deselected_items) {
    //    auto item = scene_tree_model->itemFromIndex(index);
    //
    //}


}

QStandardItem* DockSceneHierarchy::add_entity(uint64_t entity_id, bool is_root) {
    QStandardItem* existing_item = entity_map.value(entity_id, nullptr);

    // If it doesn't exist in the map
    // then this is the first time we are adding it to the map
    if (existing_item  == nullptr) {

        QStandardItem* entity_model_item = new QStandardItem();
        // Attach the entity id as the data
        entity_model_item->setData(entity_id);
        // Add the item to the map, so that when we check next time
        // we can see if the model has already been created
        entity_map.insert(entity_id, entity_model_item);
        if (is_root) {
            scene_tree_model->appendRow(entity_model_item);
        }
        return entity_model_item;
    }
    // This entity has already been created,
    // but it's possible that the name hasn't been set yet
    // this may be to do that when we are parsing the entity tree,
    // we may know the entity id, but not the name yet
    // once we know the name, we may to back patch it into the item
    return existing_item;

}

void DockSceneHierarchy::add_entity_child(uint64_t entity_id, uint64_t child_entity_id) {
    QStandardItem* existing_item = entity_map.value(entity_id, nullptr);
    QStandardItem* existing_child_item = entity_map.value(child_entity_id, nullptr);

    // If it doesn't exist in the map
    // then this is the first time we are adding it to the map
    if (existing_item  == nullptr || existing_child_item == nullptr) {
        assert(0);
    } else {
        existing_item->appendRow(existing_child_item);
    }
}



void DockSceneHierarchy::set_entity_name(uint64_t entity_id, const QString& name) {
    QStandardItem* existing_item = entity_map.value(entity_id, nullptr);
    if (existing_item  == nullptr) {
        assert(0);
    } else {
        existing_item->setText(name);
    }
}

void DockSceneHierarchy::new_entity_from_editor() {
    qDebug() << "new entity";


    QStandardItem* entity_model_item = new QStandardItem("New Entity");
    // Attach the entity id as the data
    entity_model_item->setData(1);
    scene_tree_model->appendRow(entity_model_item);


}


void DockSceneHierarchy::copy_entitys() {
    qDebug() << "copy entity";

    QModelIndexList indexes = selection_model ->selectedIndexes();
    int count = indexes.count();

    for (auto const& index: indexes) {
        auto item = scene_tree_model->itemFromIndex(index);
        uint64_t entity_id = item->data().toULongLong();
        entity_copy_list.push_back(entity_id);
        qDebug() << "copy " << item->data();
    }
}

void DockSceneHierarchy::paste_entitys() {
    qDebug() << "paste entity";
    for(uint64_t entity : entity_copy_list) {
        auto item = entity_map.value(entity);
        QStandardItem* entity_model_item = new QStandardItem(item->text() + " Copy");
        // Attach the entity id as the data
        entity_model_item->setData(entity);
        scene_tree_model->appendRow(entity_model_item);
    }
    entity_copy_list.clear();

}

void DockSceneHierarchy::duplicate_entitys() {
    qDebug() << "duplicate entity";

    QModelIndexList indexes = selection_model ->selectedIndexes();
    int count = indexes.count();

    for (auto const& index: indexes) {
        auto item = scene_tree_model->itemFromIndex(index);
        uint64_t entity_id = item->data().toULongLong();

        QStandardItem* entity_model_item = new QStandardItem(item->text() + " Copy");
        // Attach the entity id as the data
        entity_model_item->setData(entity_id);

        scene_tree_model->appendRow(entity_model_item);

    }

}

void DockSceneHierarchy::rename_entity() {
    qDebug() << "rename entity";
    QModelIndexList indexes = selection_model ->selectedIndexes();
    //auto item = scene_tree_model->itemFromIndex(indexes.at(0));
    scene_tree_view->edit(indexes.at(0));

}

void DockSceneHierarchy::delete_entitys() {
    qDebug() << "delete entity";

    while(!selection_model->selectedIndexes().isEmpty()) {
        auto model_index = selection_model->selectedIndexes().first();
        scene_tree_model->removeRow(model_index.row(), model_index.parent());
    }


}


