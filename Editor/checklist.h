#ifndef CHECKLIST_H
#define CHECKLIST_H


#include <QWidget>
#include <QComboBox>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QEvent>
#include <QStyledItemDelegate>
#include <QListView>


// Taken from https://gist.github.com/mistic100/c3b7f3eabc65309687153fe3e0a9a720
class CheckList : public QComboBox{
    Q_OBJECT

public:
    /**
     * @brief Additional value to Qt::CheckState when some checkboxes are Qt::PartiallyChecked
     */
    static const int StateUnknown = 3;

private:
    QStandardItemModel* m_model;
    ///**
    // * @brief Text displayed when no item is checked
    // */
    //QString m_noneCheckedText;
    ///**
    // * @brief Text displayed when all items are checked
    // */
    //QString m_allCheckedText;
    ///**
    // * @brief Text displayed when some items are partially checked
    // */
    //QString m_unknownlyCheckedText;
    //QString combo_text;

signals:
    void globalCheckStateChanged(int);
    void checkboxChanged(const QModelIndex&, bool);

public:
    explicit CheckList(QWidget* parent = nullptr);
    ~CheckList();

    void setComboboxText(const QString &text);

    //void setAllCheckedText(const QString &text);
    //
    //void setNoneCheckedText(const QString &text);
    //
    //void setUnknownlyCheckedText(const QString &text);

    /**
     * @brief Adds a item to the checklist (setChecklist must have been called)
     * @return the new QStandardItem
     */
    QStandardItem* addCheckItem(const QString &label, const QVariant &data, const Qt::CheckState checkState);

    /**
     * @brief Computes the global state of the checklist :
     *      - if there is no item: StateUnknown
     *      - if there is at least one item partially checked: StateUnknown
     *      - if all items are checked: Qt::Checked
     *      - if no item is checked: Qt::Unchecked
     *      - else: Qt::PartiallyChecked
     */
    int globalCheckState();

protected:
    bool eventFilter(QObject* _object, QEvent* _event);

private:
    //void updateText();

private slots:
    void on_modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void on_itemPressed(const QModelIndex &index);

public:
    class QCheckListStyledItemDelegate : public QStyledItemDelegate
    {
    public:
        QCheckListStyledItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}

        void paint(QPainter * painter_, const QStyleOptionViewItem & option_, const QModelIndex & index_) const
        {
            QStyleOptionViewItem & refToNonConstOption = const_cast<QStyleOptionViewItem &>(option_);
            refToNonConstOption.showDecorationSelected = false;
            QStyledItemDelegate::paint(painter_, refToNonConstOption, index_);
        }
    };
};


#endif // CHECKLIST_H
