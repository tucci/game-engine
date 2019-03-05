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


//EntityItemModel::EntityItemModel(QObject *parent) : QStandardItemModel (parent) {
//
//}
//
//bool EntityItemModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) {
//
//    qDebug() << "drop mime data" << data;
//    qDebug() << "action " << action;
//    qDebug() << "row " << row << " col " << column;
//    qDebug() << "parent " << parent;
//
//    auto formats = data->formats();
//
//    if (data->hasFormat("application/x-qabstractitemmodeldatalist")) {
//        QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
//        QDataStream stream(&encoded, QIODevice::ReadOnly);
//        while (!stream.atEnd())
//        {
//            int data_row, data_col;
//            QMap<int,  QVariant> roleDataMap;
//            stream >> data_row >> data_col >> roleDataMap;
//
//            auto new_item = index(data_row, data_col, parent);
//            auto item_data = itemFromIndex(new_item);
//
//            auto new_item_data = item_data->data();
//            auto new_item_text = item_data->text();
//
//            auto entity_id = new_item_data.toULongLong();
//
//            entity_map.insert(entity_id, item_data);
//
//            qDebug() << roleDataMap;
//
//
//
//
//            /* do something with the data */
//        }
//
//    }
//
//    if (data->hasFormat("application/entity_id.list")) {
//        QByteArray encodedData = data->data("application/entity_id.list");
//        QDataStream stream(&encodedData, QIODevice::ReadOnly);
//        QVariantList newItems;
//
//        int rows = 0;
//
//        while (!stream.atEnd()) {
//            QVariant entity_id_variant;
//            stream >> entity_id_variant;
//            auto entity_id = entity_id_variant.toULongLong();
//
//            auto old_item = entity_map.value(entity_id, nullptr);
//
//            auto old_data = old_item->data();
//            auto text = old_item->text();
//
//            auto model_index = index(row, column, parent);
//
//            auto new_item = itemFromIndex(model_index);
//            //entity_map.insert(entity_id, item);
//
//
//            newItems << entity_id;
//            ++rows;
//        }
//    }
//
//
//    return QStandardItemModel::dropMimeData(data, action, row, column, parent);
//}

//QStringList EntityItemModel::mimeTypes() const  {
//    QStringList types;
//    types << "application/entity_id.list";
//    //return types;
//
//    return QStandardItemModel::mimeTypes();
//}
//
//QMimeData* EntityItemModel::mimeData(const QModelIndexList &indexes) const {
//
//    QMimeData *mimeData = new QMimeData();
//    QByteArray encodedData;
//
//    QDataStream stream(&encodedData, QIODevice::WriteOnly);
//
//    foreach (const QModelIndex &index, indexes) {
//        if (index.isValid()) {
//            //QString text = data(index, Qt::DisplayRole).toString();
//            QStandardItem* item = itemFromIndex(index);
//            QVariant entityid = item->data();
//            stream << entityid;
//          }
//      }
//
//      mimeData->setData("application/entity_id.list", encodedData);
//      //return mimeData;
//
//      return QStandardItemModel::mimeData(indexes);
//}

DockSceneHierarchy::DockSceneHierarchy(QWidget* parent) : DockInterface (parent) {
    dock_widget = parent_window->findChild<CustomDockWidget*>("DockScene");
    scene_tree_view = parent_window->findChild<QTreeView*>("scene_hierarchy_treeview");
    scene_hierarchy_filter = parent_window->findChild<QLineEdit*>("scene_hierarchy_filter");




    proxy_model = new SceneHierarchyProxyModel(this->parent_window);
    scene_tree_model = new QStandardItemModel();


    proxy_model->setSourceModel(scene_tree_model);
    scene_tree_view->setModel(proxy_model);
    scene_tree_view->setContextMenuPolicy(Qt::CustomContextMenu);
    scene_tree_view->setDragDropMode(QAbstractItemView::InternalMove);
    scene_tree_view->setAcceptDrops(true);
    scene_tree_view->viewport()->installEventFilter(this);
    scene_tree_view->installEventFilter(this);
    scene_tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    create_context_menu();
    selection_model = scene_tree_view->selectionModel();


    connect(scene_tree_view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(context_menu_event(const QPoint &)));
    connect(selection_model , SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(entity_selection_changed(const QItemSelection&,const QItemSelection&)));
    connect(scene_hierarchy_filter, &QLineEdit::textChanged, proxy_model, &SceneHierarchyProxyModel::setFilterWildcard);
    connect(proxy_model, &SceneHierarchyProxyModel::items_filtered, this, &DockSceneHierarchy::after_items_filtered);

    connect(proxy_model, SIGNAL(layoutChanged(const QList<QPersistentModelIndex>&, QAbstractItemModel::LayoutChangeHint)),
            this, SLOT(tree_layout_changed(const QList<QPersistentModelIndex> &, QAbstractItemModel::LayoutChangeHint)));

}


DockSceneHierarchy::~DockSceneHierarchy()
{
    delete proxy_model;
    delete context_menu;
    delete scene_tree_model;
}

bool DockSceneHierarchy::eventFilter(QObject* o, QEvent* e) {

    QEvent::Type event_type = e->type();

    if (o == scene_tree_view->viewport()) {

        if (event_type == QEvent::DragMove) {
            //qDebug() << "drag move event " << e;
        }
        if (event_type == QEvent::Drop) {
            QDropEvent* drop = static_cast<QDropEvent*>(e);
            qDebug() << "drop event " << e;
        }
    }
    if (o == scene_tree_view) {
        if (event_type == QEvent::KeyPress) {
            QKeyEvent* key_event = static_cast<QKeyEvent*>(e);

            int key = key_event->key();
            auto modifers = key_event->modifiers();


            if (key == Qt::Key_Delete) {
                request_delete_entitys();
            }
            else if (key == Qt::Key_N && modifers.testFlag(Qt::ControlModifier) && modifers.testFlag(Qt::ShiftModifier)) {
                request_new_entity();
            }
            else if (key == Qt::Key_C && modifers.testFlag(Qt::ControlModifier)) {
                copy_entitys();
            }
            else if (key == Qt::Key_V && modifers.testFlag(Qt::ControlModifier)) {
                paste_entitys();
            }
            else if (key == Qt::Key_Z && modifers.testFlag(Qt::ControlModifier)) {
                qDebug() << "undo tree";
            }
            else if (key == Qt::Key_Y && modifers.testFlag(Qt::ControlModifier)) {
                qDebug() << "redo tree";
            }
            else if (key == Qt::Key_D && modifers.testFlag(Qt::ControlModifier)) {
                request_duplicate_entitys();
            }
            else if (key == Qt::Key_F && modifers.testFlag(Qt::ControlModifier)) {
                scene_hierarchy_filter->setFocus();
            }
            else if (key == Qt::Key_A && modifers.testFlag(Qt::ControlModifier)) {
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


    connect(new_entity_action, SIGNAL(triggered()), this, SLOT(request_new_entity()));
    connect(copy_entity_action, SIGNAL(triggered()), this, SLOT(copy_entitys()));
    connect(paste_entity_action, SIGNAL(triggered()), this, SLOT(paste_entitys()));
    connect(duplicate_entity_action, SIGNAL(triggered()), this, SLOT(request_duplicate_entitys()));
    connect(rename_entity_action, SIGNAL(triggered()), this, SLOT(rename_entity()));
    connect(delete_entity_action, SIGNAL(triggered()), this, SLOT(request_delete_entitys()));


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
     QItemSelection selected_source = proxy_model->mapSelectionToSource(selected);
     QItemSelection deselected_source = proxy_model->mapSelectionToSource(deselected);

    auto selected_items = selected_source.indexes();
    auto deselected_items = deselected_source.indexes();

    int selected_count = selected_items.count();
    bool any_selected = selected_items.count() > 0;

    duplicate_entity_action->setEnabled(any_selected);
    rename_entity_action->setEnabled(selected_count == 1);
    delete_entity_action->setEnabled(any_selected);
    copy_entity_action->setEnabled(any_selected);

    for (const QModelIndex& index : selected_items) {
        auto item = scene_tree_model->itemFromIndex(index);
        uint64_t entity_id = item->data().toULongLong();
        selected_entities.insert(entity_id);

    }

    for (const QModelIndex& index : deselected_items) {
        auto item = scene_tree_model->itemFromIndex(index);
        uint64_t entity_id = item->data().toULongLong();
        selected_entities.remove(entity_id);
    }


    //qDebug() << "selected entities: ";
    //foreach (const uint64_t entity_id, selected_entities) {
    //    qDebug() << entity_id;
    //}
    emit notify_entity_selection_change(selected_entities);

}

void DockSceneHierarchy::tree_layout_changed(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
    qDebug() << "layout changed";
    qDebug() << parents;



}




QStandardItem* DockSceneHierarchy::add_entity(uint64_t entity_id, bool is_root) {



    QStandardItem* existing_item = weak_entity_map.value(entity_id, nullptr);

    // If it doesn't exist in the map
    // then this is the first time we are adding it to the map
    if (existing_item  == nullptr) {

        QStandardItem* entity_model_item = new QStandardItem();
        // Attach the entity id as the data
        entity_model_item->setData(entity_id);
        // Add the item to the map, so that when we check next time
        // we can see if the model has already been created
        weak_entity_map.insert(entity_id, entity_model_item);
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
    QStandardItem* existing_item = weak_entity_map.value(entity_id, nullptr);
    QStandardItem* existing_child_item = weak_entity_map.value(child_entity_id, nullptr);

    // If it doesn't exist in the map
    // then this is the first time we are adding it to the map
    if (existing_item  == nullptr || existing_child_item == nullptr) {
        assert(0);
    } else {
        existing_item->appendRow(existing_child_item);
    }
}



void DockSceneHierarchy::set_entity_name(uint64_t entity_id, const QString& name) {
    QStandardItem* existing_item = weak_entity_map.value(entity_id, nullptr);
    if (existing_item  == nullptr) {
        assert(0);
    } else {
        existing_item->setText(name);
    }
}

void DockSceneHierarchy::request_new_entity() {
    emit notify_entity_new();
}

void DockSceneHierarchy::respond_new_entity(uint64_t entity_id, const QString& name) {
    QStandardItem* entity_model_item = new QStandardItem(name);
    // Attach the entity id as the data
    entity_model_item->setData(entity_id);
    scene_tree_model->appendRow(entity_model_item);
    weak_entity_map.insert(entity_id, entity_model_item);
}


void DockSceneHierarchy::respond_delete_entitys(const QList<uint64_t>& entities) {
    qDebug() << "respond_delete_entitys " << entities;
    for (uint64_t entity_id : entities) {
        QStandardItem* existing_item = weak_entity_map.value(entity_id, nullptr);
        if(existing_item == nullptr) {
            continue;
        }

        auto index = existing_item->index();
        if (index.isValid()) {
            QModelIndex index_parent = index.parent();
            scene_tree_model->removeRow(index.row(), index_parent.isValid() ? index_parent : QModelIndex());
            weak_entity_map.remove(entity_id);
        }
    }



    //QModelIndexList indexes = source.indexes();
       //    QModelIndex model_index = indexes.first();
       //    scene_tree_model->removeRow(model_index.row(), model_index.parent());
       //    // Reupdate source
       //    source = proxy_model->mapSelectionToSource(selection_model->selection());
}

void DockSceneHierarchy::respond_duplicate_entitys(const QList<uint64_t>& entities) {

    qDebug() << "respond_duplicate_entitys" << entities;

    for (uint64_t entity_id : entities) {

        if (!weak_entity_map.contains(entity_id)) {
            QStandardItem* entity_model_item = new QStandardItem("Entity " + QString::number(entity_id));
            // Attach the entity id as the data
            entity_model_item->setData(entity_id);
            weak_entity_map.insert(entity_id, entity_model_item);
            scene_tree_model->appendRow(entity_model_item);
        }

    }


    //QStandardItem* entity_model_item = new QStandardItem(item->text() + " Copy");
    //// Attach the entity id as the data
    //entity_model_item->setData(entity_id);
    //scene_tree_model->appendRow(entity_model_item);
}


void DockSceneHierarchy::copy_entitys() {
    qDebug() << "copy entity";

    // Clear previously copied items
    entity_copy_list.clear();

    auto source = proxy_model->mapSelectionToSource(selection_model->selection());
    QModelIndexList indexes = source.indexes();

    //QModelIndexList indexes = selection_model ->selectedIndexes();
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
        //auto item = entity_map.value(entity);
        //QStandardItem* entity_model_item = new QStandardItem(item->text() + " Copy");
        //// Attach the entity id as the data
        //entity_model_item->setData(entity);
        //scene_tree_model->appendRow(entity_model_item);
    }
    emit notify_entity_duplicate(entity_copy_list);

    entity_copy_list.clear();
}

void DockSceneHierarchy::request_duplicate_entitys() {
    qDebug() << "duplicate entity";

    auto source = proxy_model->mapSelectionToSource(selection_model->selection());
    QModelIndexList indexes = source.indexes();
    QList<uint64_t> entitys_to_duplicate;

    //QModelIndexList indexes = selection_model ->selectedIndexes();
    int count = indexes.count();

    entitys_to_duplicate.reserve(count);

    for (auto const& index: indexes) {
        auto item = scene_tree_model->itemFromIndex(index);

        uint64_t entity_id = item->data().toULongLong();
        entitys_to_duplicate.push_back(entity_id);

        //QStandardItem* entity_model_item = new QStandardItem(item->text() + " Copy");
        //// Attach the entity id as the data
        //entity_model_item->setData(entity_id);
        //scene_tree_model->appendRow(entity_model_item);
    }

    qDebug() << "request_duplicate_entitys " << entitys_to_duplicate;
    emit notify_entity_duplicate(entitys_to_duplicate);

}

void DockSceneHierarchy::rename_entity() {
    qDebug() << "rename entity";

    //auto source = proxy_model->mapSelectionToSource(selection_model->selection());
    //QModelIndexList indexes = source.indexes();

    QModelIndexList indexes = selection_model ->selectedIndexes();
    auto item = scene_tree_model->itemFromIndex(indexes.at(0));
    scene_tree_view->edit(indexes.at(0));

}

void DockSceneHierarchy::request_delete_entitys() {




    QItemSelection source = proxy_model->mapSelectionToSource(selection_model->selection());
    auto indexes = source.indexes();


    QList<uint64_t> entitys_to_delete;   
    entitys_to_delete.reserve(indexes.count());


    for(QModelIndex const& index: indexes) {
        QStandardItem* item = scene_tree_model->itemFromIndex(index);
        uint64_t entity_id = item->data().toULongLong();


        entitys_to_delete.push_back(entity_id);
    }
    qDebug() << "request_delete_entitys " << entitys_to_delete;
    emit notify_entity_delete(entitys_to_delete);



    //while (!source.isEmpty()) {
    //    QModelIndexList indexes = source.indexes();
    //    QModelIndex model_index = indexes.first();
    //    scene_tree_model->removeRow(model_index.row(), model_index.parent());
    //    // Reupdate source
    //    source = proxy_model->mapSelectionToSource(selection_model->selection());
    //}

    //while(!selection_model->selectedIndexes().isEmpty()) {
    //    auto model_index = selection_model->selectedIndexes().first();
    //    scene_tree_model->removeRow(model_index.row(), model_index.parent());
    //}


}


