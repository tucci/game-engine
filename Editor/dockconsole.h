#ifndef CONSOLEDOCK_H
#define CONSOLEDOCK_H

#include <QMainWindow>

#include "dockinterface.h"

#include <QPushButton>
#include <QCheckBox>
#include <QTableView>
#include <QLineEdit>
#include <customdockwidget.h>
#include "consolelistitemmodel.h"
#include "consolelogitemdelegate.h"
#include "checklist.h"


class DockConsole : public DockInterface
{
    Q_OBJECT
public:
    DockConsole(QWidget* parent);
    ~DockConsole();
    bool push_log_item(const LogItem& item);
private:
    //ConsoleLogItemDelegate* log_item_delegate;
    ConsoleFilterProxyModel* proxy_model;
    QTableView* console_tableview;
    CheckList* console_column_select;
    CheckList* console_tag_select;


    QCheckBox* info_checkbox;
    QCheckBox* warning_checkbox;
    QCheckBox* error_checkbox;
    QPushButton* console_clear_console;
    QLineEdit* console_search;
    ConsoleListItemModel* console_list_model;



private slots:
    void info_checkbox_toggled(bool toggled);
    void warning_checkbox_toggled(bool toggled);
    void error_checkbox_toggled(bool toggled);
    void console_clear_console_clicked();
    void console_table_column_toggled(const QModelIndex& item, bool checked);
    void console_tag_toggled(const QModelIndex& item, bool checked);
    void tag_added(const QString& tag);

};

#endif // CONSOLEDOCK_H
