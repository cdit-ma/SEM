//
// Created by Cathlyn on 3/02/2020.
//

#include "triggerbrowser.h"
#include "triggeritemdelegate.h"
#include "../../theme.h"

#include <QVBoxLayout>
#include <QHeaderView>

static int untitled_incr = 1;

TriggerBrowser::TriggerBrowser(QWidget* parent)
        : QWidget(parent)
{
    setupLayout();
    themeChanged();
    connect(Theme::theme(), &Theme::theme_Changed, this, &TriggerBrowser::themeChanged);
}


void TriggerBrowser::themeChanged()
{
    Theme* theme = Theme::theme();

    horizontal_splitter_->setStyleSheet(theme->getSplitterStyleSheet());
    trigger_list_view_->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    trigger_fields_view_->setStyleSheet(theme->getAbstractItemViewStyleSheet());

    toolbar_->setIconSize(theme->getIconSize());
    toolbar_->setStyleSheet(theme->getToolBarStyleSheet());

    add_trigger_action_->setIcon(theme->getIcon("Icons", "plus"));
    remove_trigger_action_->setIcon(theme->getIcon("Icons", "bin"));
}


void TriggerBrowser::currentTriggerChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.isValid()) {
        const auto& table_model = trigger_item_model_->getTableModel(current);
        if (table_model != nullptr) {
            trigger_fields_view_->setModel(table_model);
        }
    }
}


void TriggerBrowser::triggerListDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (!topLeft.isValid()) {
        return;
    }
    if (topLeft == bottomRight) {
        auto item = trigger_item_model_->itemFromIndex(topLeft);
        if (item->text().isEmpty()) {
            item->setText(trigger_item_text_);
        }
    }
}


void TriggerBrowser::addTrigger()
{
    // Clear existing selection
    trigger_list_view_->clearSelection();
    trigger_fields_view_->setModel(nullptr);

    auto item = new QStandardItem();
    trigger_item_text_ = "Trigger_" + QString::number(untitled_incr++);
    item->setText(trigger_item_text_);
    trigger_item_model_->appendRow(item);

    auto item_index = trigger_item_model_->indexFromItem(item);
    trigger_list_view_->setCurrentIndex(item_index);
    trigger_list_view_->edit(item_index);

    auto table_model = trigger_item_model_->getTableModel(item_index);
    if (table_model) {
        trigger_fields_view_->setModel(table_model);
    }

    if (!remove_trigger_action_->isEnabled()) {
        remove_trigger_action_->setEnabled(true);
    }
}


void TriggerBrowser::removeTrigger()
{
    auto item_index = trigger_list_view_->currentIndex();
    if (item_index.isValid()) {
        trigger_item_model_->removeRow(item_index.row());
    }
    if (trigger_item_model_->rowCount() == 0) {
        remove_trigger_action_->setEnabled(false);
        trigger_fields_view_->setModel(nullptr);
    }
}


void TriggerBrowser::setupLayout()
{
    trigger_item_model_ = new TriggerItemModel(this);
    trigger_list_view_ = new QListView(this);
    trigger_list_view_->setModel(trigger_item_model_);

    auto selection_model = new QItemSelectionModel(trigger_item_model_);
    trigger_list_view_->setSelectionModel(selection_model);

    trigger_fields_view_ = new QTableView(this);
    trigger_fields_view_->horizontalHeader()->setVisible(false);
    //trigger_fields_view_->setItemDelegate(new TriggerItemDelegate(this));

    auto item = new QStandardItem("First");
    //trigger_fields_view_.set

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
    horizontal_splitter_->addWidget(trigger_fields_view_);
    horizontal_splitter_->setStretchFactor(0, 0);
    horizontal_splitter_->setStretchFactor(1, 1);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0, 5, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(horizontal_splitter_,1);

    connect(add_trigger_action_, &QAction::triggered, this, &TriggerBrowser::addTrigger);
    connect(remove_trigger_action_, &QAction::triggered, this, &TriggerBrowser::removeTrigger);
    connect(selection_model, &QItemSelectionModel::currentChanged, this, &TriggerBrowser::currentTriggerChanged);

    // The trigger_item_text_ along with the signals/slots below prevent the triggers from having an empty text/label
    connect(trigger_item_model_, &TriggerItemModel::dataChanged, this, &TriggerBrowser::triggerListDataChanged);
    connect(trigger_list_view_, &QListView::doubleClicked, [&txt = trigger_item_text_, &model = trigger_item_model_](const QModelIndex& index) {
        auto item = model->itemFromIndex(index);
        txt = item->text();
    });
}
