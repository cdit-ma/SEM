#include "notificationdialog.h"
#include "notificationitem.h"


#include <QApplication>


#define ENTITY_ID "entity_ID"
#define NO_SELECTION_ID -2

#define FILTER_KEY "filter_key"
#define FILTER_DEFAULT_WIDTH 150

#define ICON_SIZE 24


/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent)
    : QFrame(parent)
{
    selectedEntityID = -1;

    setupLayout();
    //initialisePanel();
    //updateSelectionBasedButtons();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief NotificationDialog::filtersChanged
 */
void NotificationDialog::filtersChanged()
{
    OptionGroupBox* filterGroup = qobject_cast<OptionGroupBox*>(sender());
    if (filterGroup) {

        NOTIFICATION_FILTER filterKey = filterGroup->property(FILTER_KEY).value<NOTIFICATION_FILTER>();
        QList<QVariant> checkedFilters = filterGroup->getCheckedKeys();

        switch (filterKey) {
        case NOTIFICATION_FILTER::SEVERITY:
            foreach (NotificationItem* item, notificationItems) {
                int s = static_cast<int>(item->getSeverity());
                item->filtersChanged(filterKey, checkedFilters.contains(s));
            }
            break;
        case NOTIFICATION_FILTER::CATEGORY:
            foreach (NotificationItem* item, notificationItems) {
                int c = static_cast<int>(item->getCategory());
                item->filtersChanged(filterKey, checkedFilters.contains(c));
            }
            break;
        case NOTIFICATION_FILTER::TYPE:
            foreach (NotificationItem* item, notificationItems) {
                int t = static_cast<int>(item->getType());
                item->filtersChanged(filterKey, checkedFilters.contains(t));
            }
            break;
        default:
            break;
        }
    }
}


/**
 * @brief NotificationDialog::viewSelection
 * Center on or popout the corresponding entity for the selected notification item.
 */
void NotificationDialog::viewSelection()
{
    int numSelectedItems = selectedNotificationItems.count();
    if (numSelectedItems != 1) {
        return;
    }

    NotificationItem* selectedItem = selectedNotificationItems.at(0);
    int eID = selectedItem->getEntityID();
    if (sender() == centerOnAction) {
        emit centerOn(eID);
    } else if (sender() == popupAction) {
        emit popup(eID);
    }
}


/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    auto theme = Theme::theme();
    
    setStyleSheet(
                    "#NotificationDialog {background-color: " % theme->getBackgroundColorHex() + ";} " +
                    "QScrollArea {border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0);} " +
                    "QLabel {color:" + theme->getTextColorHex() + ";} " + 
                    theme->getToolBarStyleSheet() +
                    theme->getSplitterStyleSheet()
                );

    notifications_widget->setStyleSheet("background: rgba(0,0,0,0);");
    filters_widget->setStyleSheet("background: rgba(0,0,0,0);");


    center_action->setIcon(theme->getIcon("Icons", "crosshair"));
    popup_action->setIcon(theme->getIcon("Icons", "popOut"));
    //search_action->setIcon(theme->getIcon("Icons", "zoom"));
    //refresh_action->setIcon(theme->getIcon("Icons", "refresh"));

    //query_label->setStyleSheet("color:" + theme->getHighlightColorHex() + ";");
    info_label->setStyleSheet("color:" + theme->getAltBackgroundColorHex() + ";");
}


/**
 * @brief NotificationDialog::selectionChanged
 * @param item - the notification item that was clicked
 * @param selected - the item's current selected state
 * @param controlDown - control key's down state
 */
void NotificationDialog::selectionChanged(NotificationItem* item, bool selected, bool controlDown)
{
    if (!item) {
        return;
    }

    bool selectItem = !selected;
    if (!controlDown) {
        if (selectedNotificationItems.count() > 1) {
            selectItem = true;
        }
        clearSelection();
    }

    if (selectItem) {
        selectedNotificationItems.append(item);
    } else {
        selectedNotificationItems.removeAll(item);
    }

    item->setSelected(selectItem);
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::entitySelectionChanged
 * Update the selectedEntityID and the displayed notifications if necessary if this panel is visible.
 * @param ID
 */
void NotificationDialog::entitySelectionChanged(int ID)
{
    selectedEntityID = ID;
    if (this->isVisible() && displayLinkedItemsButton->isChecked()) {
        selectionFilterToggled(true);
    }
}


/**
 * @brief NotificationDialog::selectionFilterToggled
 * @param checked
 */
void NotificationDialog::selectionFilterToggled(bool checked)
{
    QToolButton* senderButton = qobject_cast<QToolButton*>(sender());
    QToolButton* otherButton = 0;

    if (senderButton == displayAllButton) {
        // don't allow the "All" button to be unchecked manually
        if (!checked) {
            displayAllButton->setChecked(true);
            return;
        }
        otherButton = displayLinkedItemsButton;
    } else {
        otherButton = displayAllButton;
    }
    otherButton->setChecked(!checked);

    bool displayAll = displayAllButton->isChecked();
    foreach (NotificationItem* item, notificationItems) {
        if (displayAll) {
            // if "All" display is checked, show all the items
            item->filtersChanged(ENTITY_ID, item->getEntityID());
        } else {
            // if there is nothing currently selected, hide the items
            if (selectedEntityID == -1) {
                item->filtersChanged(ENTITY_ID, NO_SELECTION_ID);
            } else {
                item->filtersChanged(ENTITY_ID, selectedEntityID);
            }
        }
    }
    QString newBoxTitle = "CONTEXT";
    if (!displayAll) {
        newBoxTitle += " (1)";
    }
    displayToggleBox->setTitle(newBoxTitle);
}


/**
 * @brief NotificationDialog::resetPanel
 */
void NotificationDialog::resetPanel()
{
    clearAll();
}


/**
 * @brief NotificationDialog::getLastNotificationID
 * Send a signal with the top most notification item's ID.
 */
void NotificationDialog::getLastNotificationID()
{
    if (!notificationItems.isEmpty()) {
        QLayoutItem* topItem = itemsLayout->itemAt(0);
        if (topItem) {
            NotificationItem* item = qobject_cast<NotificationItem*>(topItem->widget());
            if (item) {
                emit lastNotificationID(item->getID());
                return;
            }
        }
    }
    emit lastNotificationID(-1);
}


/**
 * @brief NotificationDialog::clearSelected
 */
void NotificationDialog::clearSelected()
{
    // delete selected items
    while (!selectedNotificationItems.isEmpty()) {
        removeItem(selectedNotificationItems.takeFirst());
    }
}


/**
 * @brief NotificationDialog::clearVisible
 */
void NotificationDialog::clearVisible()
{
    QList<NotificationItem*> visibleItems;
    foreach (NotificationItem* item, notificationItems) {
        if (item->isVisible()) {
            visibleItems.append(item);
        }
    }

    // delete visible items
    while (!visibleItems.isEmpty()) {
        removeItem(visibleItems.takeFirst());
    }
}


/**
 * @brief NotificationDialog::clearNotifications
 * @param filter
 * @param filterVal
 */
void NotificationDialog::clearNotifications(NOTIFICATION_FILTER filter, int filterVal)
{
    QList<NotificationItem*> itemsToDelete;
    foreach (NotificationItem* item, notificationItems.values()) {
        switch (filter) {
        case NOTIFICATION_FILTER::SEVERITY:
        {
            if (item->getSeverity() == (NOTIFICATION_SEVERITY)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        case NOTIFICATION_FILTER::TYPE:
        {
            if (item->getType() == (NOTIFICATION_TYPE)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        case NOTIFICATION_FILTER::CATEGORY:
        {
            if (item->getCategory() == (NOTIFICATION_CATEGORY)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        default:
            break;
        }
    }
    foreach (NotificationItem* item, itemsToDelete) {
        removeItem(item);
    }
}


/**
 * @brief NotificationDialog::notificationAdded
 * @param obj
 */
void NotificationDialog::notificationAdded(NotificationObject* obj)
{
    if (!obj) {
        return;
    }

    NotificationItem* item = new NotificationItem(obj, this);
    itemsLayout->insertWidget(0, item);
    notificationItems[obj->ID()] = item;

    // add filter for actively selected entity ID
    item->addFilter(ENTITY_ID, item->getEntityID());

    connect(item, SIGNAL(hoverEnter(int)), this, SIGNAL(itemHoverEnter(int)));
    connect(item, SIGNAL(hoverLeave(int)), this, SIGNAL(itemHoverLeave(int)));
    connect(item, &NotificationItem::itemClicked, this, &NotificationDialog::selectionChanged);

    // update the search item's visibility based on the currently checked filters
    // this also sets the correct visibility values for each of its filters
    foreach (NOTIFICATION_FILTER filter, getNotificationFilters()) {
        OptionGroupBox* group = filters.value(filter, 0);
        if (group) {
            int filterVal = item->getNotificationFilterValue(filter);
            bool showItem = group->getCheckedKeys().contains(filterVal);
            item->filtersChanged(filter, showItem);
        }
    }
    if (!displayAllButton->isChecked()) {
        item->filtersChanged(ENTITY_ID, selectedEntityID);
    }
}


/**
 * @brief NotificationDialog::notificationDeleted
 * Delete notification item with the provided ID from the hash and the items layout.
 * @param ID
 */
void NotificationDialog::notificationDeleted(int ID)
{
    if (notificationItems.contains(ID)) {
        NotificationItem* item = notificationItems.take(ID);
        selectedNotificationItems.removeAll(item);
        itemsLayout->removeWidget(item);
        updateSelectionBasedButtons();
        delete item;
    }
}


/**
 * @brief NotificationDialog::clearAll
 * NOTE: This function is the only one that doesn't use notificationDeleted to delete items.
 * It is only called when the notification panel is being reset.
 */
void NotificationDialog::clearAll()
{
    // remove widgets from the items layout
    QLayoutItem* child;
    while ((child = itemsLayout->takeAt(0)) != 0) {
        delete child;
    }

    notificationItems.clear();
    selectedNotificationItems.clear();

    // reset checked filter buttons and checked filter lists
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::clearSelection
 */
void NotificationDialog::clearSelection()
{
    foreach (NotificationItem* item, selectedNotificationItems) {
        item->setSelected(false);
    }
    selectedNotificationItems.clear();
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::initialisePanel
 * Construct items for notifications that were received before this dialog was constructed.
 * Show highlight alert and toast for the last notification item in the list.
 */
void NotificationDialog::initialisePanel()
{
    QList<NotificationObject*> notifications = NotificationManager::manager()->getNotificationItems();
    if (!notifications.isEmpty()) {
        foreach (NotificationObject* item, notifications) {
            notificationAdded(item);
        }
        //NotificationManager::manager()->showLastNotification();
    }
}


/**
 * @brief NotificationDialog::enterEvent
 * If this dialog is currently active, mark new notification as seen on mouse enter.
 * @param event
 */
void NotificationDialog::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    if (QApplication::activeWindow() == this) {
        emit mouseEntered();
    }
}


/**
 * @brief NotificationDialog::removeItem
 * This requests to delete the provided notification item.
 * Error items cannot be deleted using the notification dialog.
 * @param item
 */
void NotificationDialog::removeItem(NotificationItem* item)
{
    if (item) {
        NOTIFICATION_SEVERITY severity = item->getSeverity();
        if (severity != NOTIFICATION_SEVERITY::ERROR) {
            emit deleteNotification(item->getID());
        }
    }
}


/**
 * @brief NotificationDialog::setupLayout
 */
void NotificationDialog::setupLayout()
{
    auto right_widget = new QWidget(this);
    auto v_layout = new QVBoxLayout(right_widget);
    //Add Padding to the top
    v_layout->setContentsMargins(0, 5, 0, 0);
    v_layout->setSpacing(5);
    
    info_label = new QLabel("No Notifications.", this);
    info_label->setAlignment(Qt::AlignCenter);
    info_label->setFont(QFont(font().family(), 25));
    
    top_toolbar = new QToolBar(this);
    top_toolbar->setIconSize(QSize(16, 16));
    
    center_action = top_toolbar->addAction("Center On Notification");
    popup_action = top_toolbar->addAction("Popup On Notification");
    top_toolbar->addSeparator();
    //sort_time_action = top_toolbar->addAction("Sort by time");
    //sort_severity_action = top_toolbar->addAction("Sort by severity");


    filters_widget = new QWidget(this);
    filters_layout = new QVBoxLayout(filters_widget);
    filters_layout->setAlignment(Qt::AlignTop);
    filters_layout->setMargin(5);
    filters_layout->setSpacing(0);

    filters_scroll = new QScrollArea(this);
    filters_scroll->setWidget(filters_widget);
    filters_scroll->setWidgetResizable(true);


    
    notifications_widget = new QWidget(this);


    notifications_layout = new QVBoxLayout(notifications_widget);
    notifications_layout->setAlignment(Qt::AlignTop);
    notifications_layout->setSpacing(0);
    notifications_layout->setMargin(0);
    //Add the No Results info label to the results layout
    notifications_layout->addWidget(info_label);

    notifications_scroll = new QScrollArea(this);
    notifications_scroll->setWidget(notifications_widget);
    notifications_scroll->setWidgetResizable(true);

    v_layout->addWidget(top_toolbar);
    v_layout->addWidget(notifications_scroll, 1);


    splitter = new QSplitter(this);
    splitter->addWidget(filters_scroll);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << FILTER_DEFAULT_WIDTH << 2 * FILTER_DEFAULT_WIDTH);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->addWidget(splitter, 1);
    

    /*
    connect(center_action, &QAction::triggered, this, [=](){emit CenterOn(selected_id);});
    connect(popup_action, &QAction::triggered, this, [=](){emit Popup(selected_id);});

    connect(search_action, &QAction::triggered, this, &SearchDialog::SearchPopup);
    connect(refresh_action, &QAction::triggered, this, [=](){emit SearchQuery(query_text);});
    */
    setupFilters();
}


/**
 * @brief NotificationDialog::setupFilters
 */
void NotificationDialog::setupFilters()
{
    context_filters = new OptionGroupBox("CONTEXT", this);
    for(auto context : getNotificationContexts()){
        context_filters->addOption(QVariant::fromValue(context), getContextString(context), "Icons", getContextIcon(context));
    }
    //Hide
    context_filters->setOptionVisible(QVariant::fromValue(NOTIFICATION_CONTEXT::NOT_SELECTED), false);

    filters_layout->addWidget(context_filters);


    severity_filters = new OptionGroupBox("SEVERITY", this);
    for (auto severity : getNotificationSeverities()) {
        severity_filters->addOption(QVariant::fromValue(severity), getSeverityString(severity), "Icons", getSeverityIcon(severity));
    }
    filters_layout->addWidget(severity_filters);

    category_filters = new OptionGroupBox("CATEGORY", this);
    for (auto category : getNotificationCategories()) {
        category_filters->addOption(QVariant::fromValue(category), getCategoryString(category), "Icons", getCategoryIcon(category));
    }
    filters_layout->addWidget(category_filters);

    source_filters = new OptionGroupBox("SOURCE", this);
    for (auto type : getNotificationTypes()) {
        source_filters->addOption(QVariant::fromValue(type), getTypeString(type), "Icons", getTypeIcon(type));
    }
    filters_layout->addWidget(source_filters);

    connect(context_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(severity_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(category_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
    connect(source_filters, &OptionGroupBox::checkedOptionsChanged, this, &NotificationDialog::filtersChanged);
}


/**
 * @brief NotificationDialog::updateSelectionBasedButtons
 */
void NotificationDialog::updateSelectionBasedButtons()
{
    bool enable = !selectedNotificationItems.isEmpty();
    // TODO - Commented this out to match search centerOn and popup buttons
    //centerOnAction->setEnabled(enable);
    //popupAction->setEnabled(enable);
    clearSelectedAction->setEnabled(enable);
}
