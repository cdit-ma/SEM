//
// Created by Cathlyn on 3/02/2020.
//

#include "triggerbrowser.h"
#include "triggeritemdelegate.h"

#include <QVBoxLayout>
#include <QHeaderView>

static int untitled_incr = 1;

TriggerBrowser::TriggerBrowser(ViewController* vc, QWidget* parent)
        : QWidget(parent),
          view_controller_(vc)
{
    setupLayout();
    themeChanged();
    connect(Theme::theme(), &Theme::theme_Changed, this, &TriggerBrowser::themeChanged);
    if (vc != nullptr) {
        connect(vc, &ViewController::vc_viewItemConstructed, this, &TriggerBrowser::viewItem_constructed);
        //connect(vc, &ViewController::vc_viewItemDestructing, this, &TriggerBrowser::viewItem_destructed);
    }
}


void TriggerBrowser::themeChanged()
{
    Theme* theme = Theme::theme();

    horizontal_splitter_->setStyleSheet(theme->getSplitterStyleSheet());
    trigger_list_view_->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    trigger_table_view_->setStyleSheet(theme->getAbstractItemViewStyleSheet() +
                                        "QAbstractItemView::item {"
                                        "padding-left: 5px;"
                                        "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                                        "border-width: 1px 0px 1px 0px;"
                                        "}");

    toolbar_->setIconSize(theme->getIconSize());
    toolbar_->setStyleSheet(theme->getToolBarStyleSheet());

    add_trigger_action_->setIcon(theme->getIcon("Icons", "plus"));
    remove_trigger_action_->setIcon(theme->getIcon("Icons", "bin"));
}


void TriggerBrowser::currentTriggerChanged(const QModelIndex& current, const QModelIndex& previous)
{
    return;
    if (current.isValid()) {
        const auto& table_model = trigger_item_model_->getTableModel(current);
        if (table_model != nullptr) {
            trigger_table_view_->setModel(table_model);
            //setWaitPeriodRowVisible(table_model);
        }
    }
}


void TriggerBrowser::triggerListDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
return;
/*
    if (!topLeft.isValid()) {
        return;
    }
    if (topLeft == bottomRight) {
        auto item = trigger_item_model_->itemFromIndex(topLeft);
        if (item->text().isEmpty()) {
            item->setText(trigger_item_text_);
        }
    }*/
}


void TriggerBrowser::triggerTableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (!topLeft.isValid()) {
        return;
    }
    if (topLeft == bottomRight) {
        const auto& table_model = qobject_cast<TriggerTableModel*>(trigger_table_view_->model());
        if (table_model != nullptr) {
            //setWaitPeriodRowVisible(table_model);
        }
    }
}


void TriggerBrowser::viewItem_constructed(ViewItem* item)
{
    if (item == nullptr) {
        return;
    }
    if (!item->isNode()) {
        return;
    }
    const auto& node_item = qobject_cast<NodeViewItem*>(item);
    if (node_item->getNodeKind() == NODE_KIND::TRIGGER_DEFN) {
        addTrigger(*item);
    }
}


void TriggerBrowser::viewItem_destructed(int id, ViewItem* item)
{
    removeTrigger(id);
}


void TriggerBrowser::addTrigger(ViewItem& view_item) //int trigger_definition_id)
{
    // Clear existing selection
    trigger_list_view_->clearSelection();
    trigger_table_view_->setModel(nullptr);
    
    // Construct a new model_item and add it to the model
    auto&& item_id = view_item.getID();
    auto&& model_item_index = trigger_item_model_->addTriggerItemFor((NodeViewItem&) view_item);
    
    /*
    auto model_item = new QStandardItem(view_item.getData("label").toString());
    model_item->setData(item_id, TriggerItemModel::IDRole);
    trigger_item_model_->appendRow(model_item);
    trigger_model_items_.insert(item_id, model_item);
    */
    
    // Connect the model and view item such that their labels match when either one of them is changed
    // Only update the corresponding values if data has actually been changed
    connect(trigger_item_model_, &TriggerItemModel::itemChanged, [&vc = view_controller_, &view_item](QStandardItem* changed_model_item) {
        auto&& view_item_id = view_item.getID();
        if (changed_model_item->data(TriggerItemModel::IDRole).toInt() == view_item_id) {
            auto&& view_item_txt = view_item.getData("label").toString();
            auto&& model_item_txt = changed_model_item->text();
            if (view_item_txt != model_item_txt) {
                // If the new model label is empty, reset it to the current view item's label
                if (model_item_txt.isEmpty()) {
                    changed_model_item->setText(view_item_txt);
                } else {
                    vc->SetData(view_item_id, "label", model_item_txt);
                }
            }
        }
    });
    
    // Select the new model_item and put it on edit mode
    //auto item_index = trigger_item_model_->indexFromItem(model_item);
    trigger_list_view_->setCurrentIndex(model_item_index);
    trigger_list_view_->edit(model_item_index);
    
    // Display the new model_item's corresponding table model
    auto table_model = trigger_item_model_->getTableModel(model_item_index);
    if (table_model) {
        trigger_table_view_->setModel(table_model);
        trigger_table_view_->setRowHidden(table_model->rowCount(QModelIndex()) - 1, true);
        connect(table_model, &QAbstractTableModel::dataChanged, this, &TriggerBrowser::triggerTableDataChanged);
    }
    
    if (!remove_trigger_action_->isEnabled()) {
        remove_trigger_action_->setEnabled(true);
    }
}


void TriggerBrowser::removeTrigger(int trigger_definition_id)
{
    if (trigger_model_items_.contains(trigger_definition_id)) {
        auto trigger_item = trigger_model_items_.take(trigger_definition_id);
        trigger_item_model_->removeRow(trigger_item->row());
        if (trigger_model_items_.isEmpty()) {
            remove_trigger_action_->setEnabled(false);
            trigger_table_view_->setModel(nullptr);
        }
        delete trigger_item;
    }
    /*
    auto item_index = trigger_list_view_->currentIndex();
    if (item_index.isValid()) {
        trigger_item_model_->removeRow(item_index.row());
    }
    if (trigger_item_model_->rowCount() == 0) {
        remove_trigger_action_->setEnabled(false);
        trigger_table_view_->setModel(nullptr);
    }
    */
}


void TriggerBrowser::setWaitPeriodRowVisible(TriggerTableModel* table_model)
{
    if (trigger_table_view_->model() == table_model) {
        trigger_table_view_->setRowHidden(table_model->rowCount(QModelIndex()) - 1, table_model->singleActivation());
        trigger_table_view_->setMinimumWidth(trigger_table_view_->sizeHint().width());
    }
}


void TriggerBrowser::setupLayout()
{
    trigger_item_model_ = new TriggerItemModel(this);
    auto selection_model = new QItemSelectionModel(trigger_item_model_);

    trigger_list_view_ = new QListView(this);
    trigger_list_view_->setSpacing(5);
    trigger_list_view_->setModel(trigger_item_model_);
    trigger_list_view_->setSelectionModel(selection_model);

    trigger_table_view_ = new QTableView(this);
    trigger_table_view_->horizontalHeader()->setVisible(false);
    //trigger_table_view_->horizontalHeader()->setStretchLastSection(true);
    trigger_table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    trigger_table_view_->setItemDelegate(new TriggerItemDelegate(this));
    trigger_table_view_->setAutoScroll(true);
    trigger_table_view_->setShowGrid(false);

    toolbar_ = new QToolBar(this);
    toolbar_->setContentsMargins(0,0,0,4);

    add_trigger_action_ = toolbar_->addAction("Add Trigger");
    remove_trigger_action_ = toolbar_->addAction("Remove Trigger");
    remove_trigger_action_->setEnabled(false);

    auto trigger_list_widget = new QWidget(this);
    auto trigger_list_layout = new QVBoxLayout(trigger_list_widget);
    trigger_list_layout->setContentsMargins(5,0,0,0);
    trigger_list_layout->setSpacing(5);
    trigger_list_layout->setMargin(0);
    trigger_list_layout->addWidget(trigger_list_view_);
    trigger_list_layout->addWidget(toolbar_, 0, Qt::AlignRight);

    horizontal_splitter_ = new QSplitter(Qt::Horizontal, this);
    horizontal_splitter_->addWidget(trigger_list_widget);
    horizontal_splitter_->addWidget(trigger_table_view_);
    horizontal_splitter_->setStretchFactor(0, 0);
    horizontal_splitter_->setStretchFactor(1, 1);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0, 5, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(horizontal_splitter_,1);
    
    if (view_controller_ != nullptr) {
        connect(add_trigger_action_, &QAction::triggered, view_controller_, &ViewController::constructTriggerDefinition);
        connect(remove_trigger_action_, &QAction::triggered, [this]() {
            const auto& index = trigger_list_view_->currentIndex();
            const auto& selected_trigger = trigger_item_model_->itemFromIndex(index);
            auto&& trigger_id = selected_trigger->data(TriggerItemModel::IDRole).toInt();
            view_controller_->Delete({trigger_id});
        });
    }
    
    //connect(add_trigger_action_, &QAction::triggered, this, &TriggerBrowser::addTrigger);
    //connect(remove_trigger_action_, &QAction::triggered, this, &TriggerBrowser::removeTrigger);
    connect(selection_model, &QItemSelectionModel::currentChanged, this, &TriggerBrowser::currentTriggerChanged);

    // The trigger_item_text_ along with the signals/slots below prevent the triggers from having an empty text/label
    connect(trigger_item_model_, &TriggerItemModel::dataChanged, this, &TriggerBrowser::triggerListDataChanged);
    /*connect(trigger_list_view_, &QListView::doubleClicked, [&txt = trigger_item_text_, &model = trigger_item_model_](const QModelIndex& index) {
        auto item = model->itemFromIndex(index);
        txt = item->text();
    });*/
}
