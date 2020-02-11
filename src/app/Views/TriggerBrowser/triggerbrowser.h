//
// Created by Cathlyn on 3/02/2020.
//

#ifndef MEDEA_TRIGGERBROWSER_H
#define MEDEA_TRIGGERBROWSER_H

#include "triggeritemmodel.h"
#include "../../Controllers/ViewController/viewcontroller.h"

#include <QWidget>
#include <QListView>
#include <QTableView>
#include <QSplitter>
#include <QToolBar>

class TriggerBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerBrowser(ViewController* vc, QWidget* parent = nullptr);

private slots:
    void themeChanged();

    void selectedTriggerChanged(const QModelIndex& current, const QModelIndex& previous);
    //void triggerListDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());
    void triggerDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

    void viewItem_constructed(ViewItem* item);
    void viewItem_destructed(int id, ViewItem* item);

private:
    void addTrigger(NodeViewItem& node_item);
    void removeTrigger(int trigger_definition_id);
    
    void updateTableView(const QModelIndex& index);
    void setupLayout();

    ViewController* view_controller_ = nullptr;
    TriggerItemModel* trigger_item_model_ = nullptr;

    QListView* trigger_list_view_ = nullptr;
    QTableView* trigger_table_view_ = nullptr;
    QSplitter* horizontal_splitter_ = nullptr;

    QToolBar* toolbar_ = nullptr;
    QAction* add_trigger_action_ = nullptr;
    QAction* remove_trigger_action_ = nullptr;

    QHash<int, QStandardItem*> trigger_model_items_;
};

#endif //MEDEA_TRIGGERBROWSER_H
