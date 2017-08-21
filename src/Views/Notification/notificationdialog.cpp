#include "notificationdialog.h"
#include "notificationitem.h"

#include "../../Controllers/ViewController/viewcontroller.h"
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
NotificationDialog::NotificationDialog(ViewController* viewController, QWidget *parent)
    : QFrame(parent)
{
    selectedEntityID = -1;
    this->viewController = viewController;

    setupLayout();

    initialisePanel();

    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationDialog::themeChanged);
    if(viewController){
        connect(viewController->getSelectionController(), &SelectionController::selectionChanged, this, &NotificationDialog::selectionChanged);
    }
    themeChanged();
}


/**
 * @brief NotificationDialog::filtersChanged
 */
void NotificationDialog::filtersChanged()
{
    updateNotificationVisibility(notification_items.values());
}

void NotificationDialog::updateNotificationVisibility(QList<NotificationItem*> items){
    auto checked_context_set = context_filters->getCheckedOptions<NOTIFICATION_CONTEXT>().toSet();
    auto checked_severity_set = severity_filters->getCheckedOptions<NOTIFICATION_SEVERITY>().toSet();
    auto checked_category_set = category_filters->getCheckedOptions<NOTIFICATION_CATEGORY>().toSet();
    auto checked_type_set = source_filters->getCheckedOptions<NOTIFICATION_TYPE>().toSet();
    
    //We only need to check selection when the SELECTED is exclusively selected.
    bool check_selection = checked_context_set.size() == 1 && checked_context_set.contains(NOTIFICATION_CONTEXT::SELECTED);
    QSet<int> selected_ids;
    if(viewController && check_selection){
        selected_ids = viewController->getSelectionController()->getSelectionIDs().toSet();
    }

    for(auto item : items){
        bool matches_severity = checked_severity_set.contains(item->getSeverity());
        bool matches_category = checked_category_set.contains(item->getCategory());
        bool matches_type = checked_type_set.contains(item->getType());
        bool matches_context = true;
        if(check_selection){
            matches_context = selected_ids.contains(item->getEntityID());
        }

        item->setVisible(matches_severity && matches_category && matches_type && matches_context);
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



void NotificationDialog::ToggleSelection(NotificationItem* item){
    bool set_selected = selected_notification != item;

    
    if(selected_notification){
        //unselect old notification
        selected_notification->setSelected(!set_selected);
    }

    if(item){
        item->setSelected(set_selected);
    }
    selected_notification = item;

    
    bool has_entity = selected_notification && selected_notification->getEntityID() != -1;
    
    center_action->setEnabled(has_entity);
    popup_action->setEnabled(has_entity);
}


/**
 * @brief NotificationDialog::resetPanel
 */
void NotificationDialog::resetPanel()
{
    clearAll();
}


void NotificationDialog::selectionChanged(){
    auto checked_context_set = context_filters->getCheckedOptions<NOTIFICATION_CONTEXT>().toSet();
    if(checked_context_set.contains(NOTIFICATION_CONTEXT::SELECTED)){
        filtersChanged();
    }
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
void NotificationDialog::notificationAdded(NotificationObject* notification)
{
    if (!notification) {
        return;
    }
    auto id = notification->ID();
    if(!notification_items.contains(id)){
        auto notification_item = new NotificationItem(notification, this);

        notifications_layout->insertWidget(0, notification_item);
        notification_items[id] = notification_item;
    
        connect(notification_item, &NotificationItem::hoverEnter, this, & NotificationDialog::itemHoverEnter);
        connect(notification_item, &NotificationItem::hoverLeave, this, & NotificationDialog::itemHoverLeave);
        connect(notification_item, &NotificationItem::itemClicked, this, & NotificationDialog::ToggleSelection);

        updateNotificationVisibility({notification_item});
    }
}


/**
 * @brief NotificationDialog::notificationDeleted
 * Delete notification item with the provided ID from the hash and the items layout.
 * @param ID
 */
void NotificationDialog::notificationDeleted(int ID)
{
    if (notification_items.contains(ID)) {
        auto notification = notification_items.take(ID);
        selectedNotificationItems.removeAll(notification);
        itemsLayout->removeWidget(notification);
        updateSelectionBasedButtons();
        delete notification;
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
    for(auto notification : NotificationManager::manager()->getNotificationItems()){
        notificationAdded(notification);
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
