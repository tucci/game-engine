#include "dockconsole.h"

#include <QDebug>

DockConsole::DockConsole(QWidget* parent) : DockInterface (parent) {
    dock_widget = parent_window->findChild<CustomDockWidget*>("DockConsole");



    console_tableview = parent_window->findChild<QTableView*>("console_tableview");
    console_column_select = parent_window->findChild<CheckList*>("console_column_select");
    console_tag_select = parent_window->findChild<CheckList*>("console_tag_select");



	

    console_list_model = new ConsoleListItemModel(this->parent_window);
    proxy_model = new ConsoleFilterProxyModel(this->parent_window);
    //log_item_delegate = new ConsoleLogItemDelegate();

    proxy_model->setSourceModel(console_list_model);
    proxy_model->setDynamicSortFilter(true);
    console_tableview->setModel(proxy_model);

    console_tableview->setColumnHidden(TABLE_COLUMNS::VERBOSITY, true);
    //console_tableview->setColumnHidden(TABLE_COLUMNS::FILENAME, true);
    //console_tableview->setColumnHidden(TABLE_COLUMNS::FUNCTION, true);
    //console_tableview->setColumnHidden(TABLE_COLUMNS::LINE_NUMBER, true);
    //console_tableview->setColumnHidden(TABLE_COLUMNS::THREAD_ID, true);



    console_column_select->addCheckItem("Time", QVariant(), Qt::Checked);
    console_column_select->addCheckItem("Verbosity", QVariant(), Qt::Unchecked);
    console_column_select->addCheckItem("Tag", QVariant(), Qt::Checked);
    console_column_select->addCheckItem("Thread Id", QVariant(), Qt::Checked);
    console_column_select->addCheckItem("Filename", QVariant(), Qt::Checked);
    console_column_select->addCheckItem("Function", QVariant(), Qt::Checked);
    console_column_select->addCheckItem("Line", QVariant(), Qt::Checked);
    console_column_select->addCheckItem("Message", QVariant(), Qt::Checked);
    // For some reason this needs to be set after all the addCheckItems are called
    // this probably has to do with internal code inside QCombobox











    // Columns drop down settings
    QString column_combobox_header = "Hide/Show Columns";
    console_column_select->setComboboxText(column_combobox_header);
    console_column_select->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLength);
    // This should be set to longest possible string to account for all options and headers
    console_column_select->setMinimumContentsLength(column_combobox_header.length());
    connect(console_column_select, &CheckList::checkboxChanged, this, &DockConsole::console_table_column_toggled);

    // Tag drop down settings
    QString tag_combobox_header = "Tag Filter";
    console_tag_select->setComboboxText(tag_combobox_header);
    console_tag_select->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLength);
    // This should be set to longest possible string to account for all options and headers
    console_tag_select->setMinimumContentsLength(tag_combobox_header.length());

    connect(console_list_model, &ConsoleListItemModel::possible_new_tag_added, this, &DockConsole::tag_added);
    connect(console_tag_select, &CheckList::checkboxChanged, this, &DockConsole::console_tag_toggled);









    //console_tableview->setModel(console_list_model);
    //console_listview->setModel(proxy_model);
    //console_listview->setModel(console_list_model);
    //console_listview->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //console_listview->setItemDelegate(log_item_delegate);



    info_checkbox = parent_window->findChild<QCheckBox*>("console_info_checkbox");
    warning_checkbox = parent_window->findChild<QCheckBox*>("console_warning_checkbox");;
    error_checkbox = parent_window->findChild<QCheckBox*>("console_error_checkbox");
    console_clear_console = parent_window->findChild<QPushButton*>("console_clear_console");
    console_search = parent_window->findChild<QLineEdit*>("console_search");

    info_checkbox->setChecked(true);
    warning_checkbox->setChecked(true);
    error_checkbox->setChecked(true);

    connect(info_checkbox, &QCheckBox::clicked, this, &DockConsole::info_checkbox_toggled);
    connect(warning_checkbox, &QCheckBox::clicked, this, &DockConsole::warning_checkbox_toggled);
    connect(error_checkbox, &QCheckBox::clicked, this, &DockConsole::error_checkbox_toggled);

    connect(console_clear_console, &QPushButton::clicked, this, &DockConsole::console_clear_console_clicked);
    connect(console_search, &QLineEdit::textChanged, proxy_model, &ConsoleFilterProxyModel::setFilterWildcard);

    // Auto scroll the log items as they come in
    connect(console_tableview->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), console_tableview, SLOT(scrollToBottom()));
}

DockConsole::~DockConsole() {
    //delete log_item_delegate;
    delete console_list_model;
    delete proxy_model;
}

bool DockConsole::push_log_item(const LogItem& item) {
    return console_list_model->push_log_item(item);
}

void DockConsole::info_checkbox_toggled(bool toggled) {
    proxy_model->toggle_info_filter(toggled);

}
void DockConsole::warning_checkbox_toggled(bool toggled) {
    proxy_model->toggle_warning_filter(toggled);

}

void DockConsole::error_checkbox_toggled(bool toggled) {
    proxy_model->toggle_error_filter(toggled);

}

void DockConsole::console_clear_console_clicked() {
    console_list_model->clear_log();
}

void DockConsole::console_table_column_toggled(const QModelIndex& item, bool checked) {
    console_tableview->setColumnHidden(item.row(), !checked);
}

void DockConsole::console_tag_toggled(const QModelIndex& item, bool checked) {

    proxy_model->toggle_tag(item.data().toString(), checked);
}

void DockConsole::tag_added(const QString& tag) {
    if (!proxy_model->tag_exists(tag)) {
        console_tag_select->addCheckItem(tag, QVariant(), Qt::Checked);
        proxy_model->tag_added(tag);
    }

}


