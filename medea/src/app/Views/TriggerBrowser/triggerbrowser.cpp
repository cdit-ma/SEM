//
// Created by Cathlyn on 3/02/2020.
//

#include "triggerbrowser.h"
#include "triggeritemdelegate.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <keynames.h>

/**
 * @brief TriggerBrowser::TriggerBrowser
 * @param vc
 * @param parent
 */
TriggerBrowser::TriggerBrowser(ViewController* vc, QWidget* parent)
        : QWidget(parent),
          view_controller_(vc)
{
    setupLayout();
    themeChanged();
    connect(Theme::theme(), &Theme::theme_Changed, this, &TriggerBrowser::themeChanged);
    
    if (vc != nullptr) {
        connect(vc, &ViewController::vc_viewItemConstructed, this, &TriggerBrowser::viewItem_constructed);
        connect(vc, &ViewController::vc_viewItemDestructing, this, &TriggerBrowser::viewItem_destructed);
    }
}


/**
 * @brief TriggerBrowser::themeChanged
 */
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


/**
 * @brief TriggerBrowser::triggerSelectionChanged
 * This is called when the selected trigger in the list view is changed.
 * It updates the table view to display the trigger's corresponding data table.
 * @param current
 * @param previous
 */
void TriggerBrowser::triggerSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.isValid()) {
        const auto& table_model = trigger_item_model_->getTableModel(current);
        if (table_model != nullptr) {
            trigger_table_view_->setModel(table_model);
            updateTableView(current);
        }
    }
}


/**
 * @brief TriggerBrowser::triggerDataChanged
 * This is called when the data of a trigger model item in the list view has changed.
 * It is used to catch a change in the Label and SingleActivation key values.
 * @param topLeft
 * @param bottomRight
 * @param roles
 */
void TriggerBrowser::triggerDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (!topLeft.isValid()) {
        return;
    }
    if (roles.contains(Qt::DisplayRole) || roles.contains(Qt::EditRole)) {
        // This is when the displayed value in the view has changed - in this case, a label change
        for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
            auto&& model_item = trigger_item_model_->item(i, 0);
            auto&& model_item_txt = model_item->text();
            auto&& view_item_id = model_item->data(TriggerItemModel::IDRole).toInt();
            auto&& view_item_txt = view_controller_->getEntityDataValue(view_item_id, KeyName::Label).toString();
            // If the new model label is empty, reset it to the current view item's label
            if (model_item_txt.isEmpty()) {
                model_item->setText(view_item_txt);
            } else if (view_item_txt != model_item_txt) {
                view_controller_->SetData(view_item_id, KeyName::Label, model_item_txt);
            }
        }
    }
    if (roles.contains(TriggerItemModel::SingleActivationRole)) {
        // Changing this value shows or hides the WaitPeriod row;
        updateTableView(topLeft);
    }
}


/**
 * @brief TriggerBrowser::viewItem_constructed
 * This is called when a new ViewItem has been constructed.
 * If the constructed item is a Trigger definition, this adds a corresponding model item in the list view.
 * @param item
 */
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
        addTrigger(*node_item);
    }
}


/**
 * @brief TriggerBrowser::viewItem_destructed
 * This is called when a ViewItem has been destructed.
 * If there is a model item tied to the destructed item, remove it.
 * @param id
 * @param item
 */
void TriggerBrowser::viewItem_destructed(int id, ViewItem* item)
{
    removeTrigger(id);
}


/**
 * @brief TriggerBrowser::addTrigger
 * This constructs, sets up and adds a model item for the provided NodeViewItem.
 * It also selects the new model item and displays its corresponding data table.
 * @param node_item
 */
void TriggerBrowser::addTrigger(NodeViewItem& node_item)
{
    // Clear existing selection
    trigger_list_view_->clearSelection();
    trigger_table_view_->setModel(nullptr);
    
    // Construct a new model_item and add it to the model
    auto&& item_model_index = trigger_item_model_->addItemForTriggerDefinition(node_item);
    
    // Select the new model item and put it on edit mode if it was added by the user
    trigger_list_view_->setCurrentIndex(item_model_index);
    if (user_added_) {
        trigger_list_view_->edit(item_model_index);
        user_added_ = false;
    }
    
    // Display the new model item's corresponding data table model
    const auto& table_model = trigger_item_model_->getTableModel(item_model_index);
    if (table_model) {
        trigger_table_view_->setModel(table_model);
        updateTableView(item_model_index);
    }
    
    // Enable the remove trigger button
    if (!remove_trigger_action_->isEnabled()) {
        remove_trigger_action_->setEnabled(true);
    }
}


/**
 * @brief TriggerBrowser::removeTrigger
 * This removes the model item linked to the ViewItem with the provided id (if there is one).
 * @param trigger_definition_id
 */
void TriggerBrowser::removeTrigger(int trigger_definition_id)
{
    bool success = trigger_item_model_->removeItemForTriggerDefinition(trigger_definition_id);
    if (success) {
        if (trigger_item_model_->rowCount() == 0) {
            // If the list is empty, disable the remove trigger button and clear the table view
            remove_trigger_action_->setEnabled(false);
            trigger_table_view_->setModel(nullptr);
        } else {
            // Otherwise, select the last item in the list - this also updates the table view
            auto &&last_item_index = trigger_item_model_->index(trigger_item_model_->rowCount() - 1, 0);
            trigger_list_view_->setCurrentIndex(last_item_index);
        }
    }
}


/**
 * @brief TriggerBrowser::updateTableView
 * This is called when the selected trigger in the list has changed, or when a trigger model item's data has changed.
 * It checks the value of the SingleActivation row and shows/hides the WaitPeriod row accordingly.
 * @param index
 */
void TriggerBrowser::updateTableView(const QModelIndex& index)
{
    if (index == trigger_list_view_->currentIndex()) {
        bool hide_row = index.data(TriggerItemModel::SingleActivationRole).toBool();
        int row = index.data(TriggerItemModel::WaitPeriodRowRole).toInt();
        trigger_table_view_->setRowHidden(row, hide_row);
    }
}


/**
 * @brief TriggerBrowser::setupLayout
 */
void TriggerBrowser::setupLayout()
{
    trigger_item_model_ = new TriggerItemModel(this);
    connect(trigger_item_model_, &TriggerItemModel::dataChanged, this, &TriggerBrowser::triggerDataChanged);
    
    // This sends a signal whenever the selected trigger model item has changed
    auto selection_model = new QItemSelectionModel(trigger_item_model_);
    connect(selection_model, &QItemSelectionModel::currentChanged, this, &TriggerBrowser::triggerSelectionChanged);
    
    trigger_list_view_ = new QListView(this);
    trigger_list_view_->setSpacing(5);
    trigger_list_view_->setModel(trigger_item_model_);
    trigger_list_view_->setSelectionModel(selection_model);
    
    trigger_table_view_ = new QTableView(this);
    trigger_table_view_->horizontalHeader()->setVisible(false);
    trigger_table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    trigger_table_view_->setItemDelegate(new TriggerItemDelegate(this));
    trigger_table_view_->setAutoScroll(true);
    trigger_table_view_->setShowGrid(false);
    
    // Set a reasonable minimum for the table
    trigger_table_view_->setMinimumWidth(fontMetrics().horizontalAdvance(KeyName::SingleActivation) * 2);

    toolbar_ = new QToolBar(this);
    toolbar_->setContentsMargins(0,0,0,4);

    add_trigger_action_ = toolbar_->addAction("Add Trigger");
    remove_trigger_action_ = toolbar_->addAction("Remove Trigger");
    remove_trigger_action_->setEnabled(false);
    
    // Connect the add/remove trigger signals to the ViewController
    if (view_controller_ != nullptr) {
        connect(add_trigger_action_, &QAction::triggered, [this]() {
            user_added_ = true;
            emit view_controller_->constructTriggerDefinition();
        });
        connect(remove_trigger_action_, &QAction::triggered, [this]() {
            const auto& index = trigger_list_view_->currentIndex();
            const auto& selected_trigger = trigger_item_model_->itemFromIndex(index);
            auto&& trigger_id = selected_trigger->data(TriggerItemModel::IDRole).toInt();
            view_controller_->Delete({trigger_id});
        });
    }

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
}
