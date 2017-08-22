#include "notificationdialog.h"
#include "notificationitem.h"

#include "../../Controllers/ViewController/viewcontroller.h"
#include <QApplication>


#define FILTER_DEFAULT_WIDTH 150
#define ICON_SIZE 24


/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(ViewController* viewController, QWidget *parent)
    : QFrame(parent)
{
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

    updateVisibleCount();
}

void NotificationDialog::updateVisibleCount(){
    int visible_count = 0;
    for(auto item : notification_items){
        if(item->isVisibleTo(notifications_widget)){
            visible_count ++;
        }
    }

    auto notification_count = notification_items.size();
    bool all_visible = visible_count == notification_count;
    if(!all_visible){
        status_label->setText("[" + QString::number(notification_count - visible_count) + "/" + QString::number(notification_count) + "] notifications hidden by filters");
    }
    //Hide and show the 
    notifications_status_widget->setVisible(!all_visible);

    info_label->setVisible(notification_count == 0);
}

/**
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    auto theme = Theme::theme();
    
    setStyleSheet(
                    "#NotificationDialog {background-color: " % theme->getBackgroundColorHex() + ";}" +
                    "QScrollArea {border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0); } " +
                    "QLabel {color:" + theme->getTextColorHex() + ";} " + 
                    theme->getToolBarStyleSheet() +
                    theme->getSplitterStyleSheet() +
                    "QToolButton::checked:!hover{background: " % theme->getAltBackgroundColorHex() % ";}"
                );

    notifications_widget->setStyleSheet("background: rgba(0,0,0,0);");
    filters_widget->setStyleSheet("background: rgba(0,0,0,0);");


    center_action->setIcon(theme->getIcon("Icons", "crosshair"));
    popup_action->setIcon(theme->getIcon("Icons", "popOut"));
    sort_time_action->setIcon(theme->getIcon("Icons", "sort", true));
    reset_filters_action->setIcon(theme->getIcon("Icons", "cross"));
    
    auto pixmap = Theme::theme()->getImage("Icons", "clock", QSize(16,16), theme->getMenuIconColor());
    clock_label->setPixmap(pixmap);

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


void NotificationDialog::selectionChanged(){
    auto checked_context_set = context_filters->getCheckedOptions<NOTIFICATION_CONTEXT>().toSet();
    if(checked_context_set.contains(NOTIFICATION_CONTEXT::SELECTED)){
        filtersChanged();
    }
}

void NotificationDialog::toggleSort(){
    auto sort_ascending = sort_time_action->isChecked();
    auto ordered_list = notification_items.values();

    qSort(ordered_list.begin(), ordered_list.end(), 
    [sort_ascending](const NotificationItem* a, const NotificationItem* b) -> bool {
        bool agtb = a->getNotification()->time() > b->getNotification()->time();
         return sort_ascending ? agtb : !agtb;
        });
    
    for(auto item : ordered_list){
        notifications_layout->removeWidget(item);
        notifications_layout->addWidget(item);
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

        //Add or insert based on state of sort
        int position = sort_time_action->isChecked() ? 0 : -1;
        
        notifications_layout->insertWidget(position, notification_item);
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
        if(selected_notification == notification){
            ToggleSelection(0);
        }
        delete notification;
    }
    updateVisibleCount();
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
 * @brief NotificationDialog::setupLayout
 */
void NotificationDialog::setupLayout()
{

    auto left_widget = new QWidget(this);
    auto right_widget = new QWidget(this);

    {
        //LEFT WIDGET
        left_widget->setContentsMargins(5,5,1,5);
        auto v_layout = new QVBoxLayout(left_widget);
        v_layout->setMargin(0);

        filters_widget = new QWidget(this);
        filters_widget->setContentsMargins(5,0,5,5);
        filters_layout = new QVBoxLayout(filters_widget);
        filters_layout->setAlignment(Qt::AlignTop);
        filters_layout->setMargin(0);
        filters_layout->setSpacing(0);

        filters_scroll = new QScrollArea(this);
        filters_scroll->setWidget(filters_widget);
        filters_scroll->setWidgetResizable(true);

        v_layout->addWidget(filters_scroll, 1);
    }

    {
        //RIGHT WIDGET
        right_widget->setContentsMargins(1,5,5,5);
        auto v_layout = new QVBoxLayout(right_widget);
        v_layout->setMargin(0);
        v_layout->setSpacing(5);

        top_toolbar = new QToolBar(this);
        top_toolbar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
        top_toolbar->setIconSize(QSize(16, 16));

        clock_label = new QLabel(this);
        clock_label->setFixedSize(QSize(16, 16));
        clock_label->setAlignment(Qt::AlignCenter);
        clock_label->setToolTip("Sort Notifications by time (Ascending/Descending)");
        
        top_toolbar->addWidget(clock_label);
        sort_time_action = top_toolbar->addAction("Sort by time");
        sort_time_action->setToolTip(clock_label->toolTip());
        sort_time_action->setCheckable(true);
        sort_time_action->setChecked(true);



        
        top_toolbar->addSeparator();
        center_action = top_toolbar->addAction("Center On Notification");
        popup_action = top_toolbar->addAction("Popup On Notification");
    

        info_label = new QLabel("No notifications", this);
        info_label->setAlignment(Qt::AlignCenter);
        info_label->setFont(QFont(font().family(), 25));
        info_label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        
        //Used for the ScrollArea
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


        {
            //Is the Notification label in the status bar
            status_label = new QLabel(this);
            status_label->setAlignment(Qt::AlignCenter);
            status_label->setFont(QFont(font().family(), 12));
            status_label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));

            bottom_toolbar = new QToolBar(this);
            bottom_toolbar->setIconSize(QSize(16, 16));
            bottom_toolbar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
            reset_filters_action = bottom_toolbar->addAction("Reset Filters");

            notifications_status_widget = new QWidget(this);
            notifications_status_widget->hide();
            auto status_layout = new QHBoxLayout(notifications_status_widget);
            status_layout->setContentsMargins(0, 0, 0, 0);
            status_layout->addWidget(status_label, 1);
            status_layout->addWidget(bottom_toolbar);
        }


        //Right
        v_layout->addWidget(top_toolbar, 0, Qt::AlignRight);
        v_layout->addWidget(notifications_scroll, 1);
        v_layout->addWidget(notifications_status_widget);
    }

    splitter = new QSplitter(this);
    splitter->addWidget(left_widget);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << FILTER_DEFAULT_WIDTH << 2 * FILTER_DEFAULT_WIDTH);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(splitter, 1);
    setupFilters();

    connect(sort_time_action, &QAction::triggered, this, &NotificationDialog::toggleSort);
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
    //Hide the NOT_SELECTED option
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
    category_filters->setOptionVisible(QVariant::fromValue(NOTIFICATION_CATEGORY::NONE), false);
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



    connect(reset_filters_action, &QAction::triggered, context_filters, &OptionGroupBox::reset);
    connect(reset_filters_action, &QAction::triggered, severity_filters, &OptionGroupBox::reset);
    connect(reset_filters_action, &QAction::triggered, category_filters, &OptionGroupBox::reset);
    connect(reset_filters_action, &QAction::triggered, source_filters, &OptionGroupBox::reset);
}