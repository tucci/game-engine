#ifndef CONSOLELISTITEMMODEL_H
#define CONSOLELISTITEMMODEL_H


#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include "logitem.h"
#include <QHash>





class ConsoleFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    ConsoleFilterProxyModel(QObject* parent = 0);

    void toggle_info_filter(bool toggled);
    void toggle_warning_filter(bool toggled);
    void toggle_error_filter(bool toggled);
    void toggle_tag(const QString& tag, bool toggled);
    void tag_added(const QString& tag);
    bool tag_exists(const QString& tag);
protected:
    bool info_filter_on;
    bool warning_filter_on;
    bool error_filter_on;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    QHash<QString, bool> tag_filter_map;
};

class ConsoleListItemModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ConsoleListItemModel(QObject *parent = nullptr);


    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;


    bool push_log_item(const LogItem& item);
    void clear_log();
signals:

    void possible_new_tag_added(const QString&);

private:
    QList<LogItem> log_list;
};

#endif // CONSOLELISTITEMMODEL_H
