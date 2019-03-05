#include "consolelistitemmodel.h"
#include <QDebug>
#include <QDateTime>



ConsoleFilterProxyModel::ConsoleFilterProxyModel(QObject* parent) : QSortFilterProxyModel (parent) {
    setDynamicSortFilter(true);
    info_filter_on = true;
    warning_filter_on = true;
    error_filter_on = true;
}


void ConsoleFilterProxyModel::toggle_info_filter(bool toggled) {
    info_filter_on = toggled;
    invalidateFilter();
}

void ConsoleFilterProxyModel::toggle_warning_filter(bool toggled) {
    warning_filter_on = toggled;
    invalidateFilter();
}

void ConsoleFilterProxyModel::toggle_error_filter(bool toggled) {
    error_filter_on = toggled;
    invalidateFilter();
}

void ConsoleFilterProxyModel::toggle_tag(const QString& tag, bool toggled) {
    tag_filter_map.insert(tag, toggled);
    invalidateFilter();
}

void ConsoleFilterProxyModel::tag_added(const QString& tag) {
    tag_filter_map.insert(tag, true);
}

bool ConsoleFilterProxyModel::tag_exists(const QString& tag) {
    return tag_filter_map.contains(tag);
}

bool ConsoleFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  {


    QModelIndex verbosity_index = sourceModel()->index(sourceRow, TABLE_COLUMNS::VERBOSITY, sourceParent);
    QModelIndex tag_index = sourceModel()->index(sourceRow, TABLE_COLUMNS::TAG, sourceParent);
    QModelIndex msg_index = sourceModel()->index(sourceRow, TABLE_COLUMNS::MESSAGE, sourceParent);

    QModelIndex filename_index = sourceModel()->index(sourceRow, TABLE_COLUMNS::FILENAME, sourceParent);
    QModelIndex function_index = sourceModel()->index(sourceRow, TABLE_COLUMNS::FUNCTION, sourceParent);

    QVariant verbosity_variant = sourceModel()->data(verbosity_index);
    QVariant tag_variant = sourceModel()->data(tag_index);
    QVariant msg_variant = sourceModel()->data(msg_index);
    QVariant filename_variant = sourceModel()->data(filename_index);
    QVariant function_variant = sourceModel()->data(function_index);

    LoggerVerbosity verbosity = verbosity_variant.value<LoggerVerbosity>();
    QString tag_str = tag_variant.toString();
    QString msg_str = msg_variant.toString();

    QString filename_str = filename_variant.toString();
    QString function_str = function_variant.toString();


    //LogItem log_item = variant.value<LogItem>();
    QRegExp filter = filterRegExp();
    filter.setCaseSensitivity(Qt::CaseInsensitive);

    // Filter out tags that are not checked
    if (tag_filter_map.value(tag_str, true) == false) {
        return false;
    }

    // Filter by msg, tag, filename, function
    if (!msg_str.contains(filter)
            && !filename_str.contains(filter)
            && !function_str.contains(filter)) {
            return false;
    }

    if (info_filter_on && verbosity == LoggerVerbosity::INFO) {
            return true;
    }
    if (warning_filter_on && verbosity == LoggerVerbosity::WARN) {
            return true;
    }
    if (error_filter_on && verbosity == LoggerVerbosity::FATAL) {
            return true;
    }
    return false;

}



ConsoleListItemModel::ConsoleListItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{


}


QVariant ConsoleListItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch (section) {



        case TABLE_COLUMNS::TIME:
            return tr("Time");
        case TABLE_COLUMNS::VERBOSITY:
            return tr("Verbosity");
        case TABLE_COLUMNS::TAG:
            return tr("Tag");
        case TABLE_COLUMNS::THREAD_ID:
            return tr("Thread id");
        case TABLE_COLUMNS::FILENAME:
            return tr("File");
        case TABLE_COLUMNS::FUNCTION:
            return tr("Function");
        case TABLE_COLUMNS::LINE_NUMBER:
            return tr("Line");
        case TABLE_COLUMNS::MESSAGE:
            return tr("Message");
           default:
               return QVariant();
       }
    }
    return QVariant();
}

int ConsoleListItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    //if (parent.isValid()) {
    //    return 0;
    //}

    return log_list.count();
}

int ConsoleListItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return COLUMN_COUNT;
}

QVariant ConsoleListItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row_index = index.row();
    if (row_index >= log_list.size()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        const LogItem& item = log_list.at(row_index);

        if (index.column() == TABLE_COLUMNS::TIME) {

            QDateTime time = QDateTime::fromMSecsSinceEpoch(item.time);

            //qDebug() << "time " << time.toString();
            return time.toString("hh:mm:ss.zzz");
        }
        else if (index.column() == TABLE_COLUMNS::VERBOSITY) {
            QVariant variant;
            variant.setValue(item.verbosity);
            return variant;
        }
        else if (index.column() == TABLE_COLUMNS::TAG) {
            return item.tag;
        }
        else if (index.column() == TABLE_COLUMNS::THREAD_ID) {
            return item.thread_id;
        }
        else if (index.column() == TABLE_COLUMNS::FILENAME) {
            return item.filename_str;
        }
        else if (index.column() == TABLE_COLUMNS::FUNCTION) {
            return item.function_str;
        }
        else if (index.column() == TABLE_COLUMNS::LINE_NUMBER) {
            return item.line;
        }
        else if (index.column() == TABLE_COLUMNS::MESSAGE) {
            return item.msg_str;
        }
    }
    return QVariant();
}

bool ConsoleListItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();
    return true;
}


bool ConsoleListItemModel::push_log_item(const LogItem& item) {
    LogItem copy(item);


    int row = rowCount();

    beginInsertRows(QModelIndex(), row, row + 1 - 1);
    log_list.push_back(copy);
    endInsertRows();

    emit possible_new_tag_added(item.tag);


    return true;
}

void ConsoleListItemModel::clear_log() {
    beginResetModel();
    log_list.clear();
    endResetModel();
}
