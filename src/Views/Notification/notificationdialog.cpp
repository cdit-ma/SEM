#include "notificationdialog.h"
#include "notificationitem.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QTime>
#include <QMovie>
#include <QStringBuilder>
#include <QApplication>
#include <QGroupBox>

#define FILTER_DEFAULT_WIDTH 200

#define ICON_SIZE 24
#define SHOW_SEPARATOR_MARGIN false

#define BLINK_INTERVAL 0
#define BLINK_TIME 3000


/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent)
    : QWidget(parent)
{
    selectedEntityID = -1;

    setupLayout();
    initialisePanel();
    updateSelectionBasedButtons();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(on_themeChanged()));
    on_themeChanged();
}


/**
 * @brief NotificationDialog::on_filtersChanged
 * This is called when one of the filter buttons is triggered.
 * It sends a checked list of the triggered filter group to the notification items.
 * @param checkedKeys
 */
void NotificationDialog::on_filtersChanged(QList<QVariant> checkedKeys)
{
    FilterGroup* filterGroup = qobject_cast<FilterGroup*>(sender());
    if (filterGroup) {
        NOTIFICATION_FILTER filter = filterGroup->getFilterGroupKey().value<NOTIFICATION_FILTER>();
        emit filtersChanged(filter, checkedKeys);
    }
}


/**
 * @brief NotificationDialog::viewSelection
 * Center on or popout the corresponding entity for the selected notification item.
 */
void NotificationDialog::viewSelection()
{
    int numSelectedItems = selectedItems.count();

    if (numSelectedItems != 1) {
        if (selectedItems.isEmpty()) {
            infoLabel->setText("Please select <u>one</u> notification...");
            blinkInfoLabel();
        }
        return;
    }

    NotificationItem* selectedItem = selectedItems.at(0);
    int eID = selectedItem->getEntityID();
    if (sender() == centerOnAction) {
        emit centerOn(eID);
    } else if (sender() == popupAction) {
        emit popup(eID);
    }
}


/**
 * @brief NotificationDialog::on_themeChanged
 */
void NotificationDialog::on_themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QScrollArea {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  "QLabel{ background: rgba(0,0,0,0); color:" + theme->getTextColorHex()+ ";}"
                  + theme->getAltAbstractItemViewStyleSheet()
                  + theme->getDialogStyleSheet());

    mainWidget->setStyleSheet("background:" + theme->getBackgroundColorHex() + ";");

    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; } QToolButton{ padding: 4px; }");
    bottomToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; } QToolButton{ padding: 4px; }");
    filtersToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                                  "QToolBar{ padding: 0px; spacing: 0px; }"
                                  "QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}");

    resetFiltersAction->setIcon(theme->getIcon("Icons", "cross"));
    sortTimeAction->setIcon(theme->getIcon("Icons", "clock"));
    sortSeverityAction->setIcon(theme->getIcon("Icons", "letterAZ"));
    centerOnAction->setIcon(theme->getIcon("Icons", "crosshair"));
    popupAction->setIcon(theme->getIcon("Icons", "popOut"));
    clearSelectedAction->setIcon(theme->getIcon("Icons", "bin"));
    clearVisibleAction->setIcon(theme->getIcon("Icons", "cross"));

    displaySplitter->setStyleSheet(theme->getSplitterStyleSheet());
}


/**
 * @brief NotificationDialog::on_selectionChanged
 * @param item - the notification item that was clicked
 * @param selected - the item's current selected state
 * @param controlDown - control key's down state
 */
void NotificationDialog::on_selectionChanged(NotificationItem* item, bool selected, bool controlDown)
{
    if (!item) {
        return;
    }

    bool selectItem = !selected;
    if (!controlDown) {
        if (selectedItems.count() > 1) {
            selectItem = true;
        }
        clearSelection();
    }

    if (selectItem) {
        selectedItems.append(item);
    } else {
        selectedItems.removeAll(item);
    }

    blinkInfoLabel(false);
    item->setSelected(selectItem);
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::entitySelectionChanged
 * @param ID
 */
void NotificationDialog::entitySelectionChanged(int ID)
{
    if (ID == selectedEntityID) {
        return;
    }
    foreach (NotificationItem* item, notificationItems) {
        int entityID = item->getEntityID();
        if (entityID == ID) {
            selectedEntityItemIDs.append(entityID);
        }
    }
}


/**
 * @brief NotificationDialog::selectionFilterToggled
 * @param checked
 */
void NotificationDialog::selectionFilterToggled(bool checked)
{
    if (checked) {
        foreach (NotificationItem* item, notificationItems) {
            int entityID = item->getEntityID();
            if (selectedEntityItemIDs.contains(entityID)) {
                // show the item if it matches the current filters
            } else {
                item->setVisible(false);
            }
        }
    } else {
        foreach (NotificationItem* item, notificationItems) {
            // show all items that match the current filters
        }
    }
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
    // start/restart timer
    if (selectedItems.isEmpty()) {
        infoLabel->setText("Please select at least one notification...");
        blinkInfoLabel();
        return;
    }

    // delete selected items
    while (!selectedItems.isEmpty()) {
        removeItem(selectedItems.takeFirst());
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

    connect(item, SIGNAL(hoverEnter(int)), this, SIGNAL(itemHoverEnter(int)));
    connect(item, SIGNAL(hoverLeave(int)), this, SIGNAL(itemHoverLeave(int)));
    connect(item, &NotificationItem::itemClicked, this, &NotificationDialog::on_selectionChanged);
    connect(this, &NotificationDialog::filtersChanged, item, &NotificationItem::on_filtersChanged);

    // update the item's visibility depending on the currently checked filters
    foreach (NOTIFICATION_FILTER filter, getNotificationFilters()) {
        auto filter_group = filterGroups.value(filter, 0);
        if(filter_group){
            item->on_filtersChanged(filter, filter_group->getCheckedFilterKeys());
        }
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
        selectedItems.removeAll(item);
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
    selectedItems.clear();

    // reset checked filter buttons and checked filter lists
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::clearSelection
 */
void NotificationDialog::clearSelection()
{
    foreach (NotificationItem* item, selectedItems) {
        item->setSelected(false);
    }
    selectedItems.clear();
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
        NotificationManager::manager()->showLastNotification();
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
 * @brief NotificationDialog::blinkInfoLabel
 * @param blink
 */
void NotificationDialog::blinkInfoLabel(bool blink)
{
    if (blink) {
        intervalTime = 0;
        blinkTimer->start();
    } else {
        blinkTimer->stop();
    }
    infoAction->setVisible(blink);
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
 * @brief NotificationDialog::intervalTimeout
 */
void NotificationDialog::intervalTimeout()
{
    if (BLINK_INTERVAL == 0) {
        blinkInfoLabel(false);
    }  else {
        intervalTime += BLINK_INTERVAL;
        if (intervalTime == BLINK_TIME) {
            blinkInfoLabel(false);
        } else {
            infoAction->setVisible(!infoAction->isVisible());
        }
    }
}


/**
 * @brief NotificationDialog::setupLayout
 */
void NotificationDialog::setupLayout()
{
    /*
     * TOP TOOLBAR
     */
    topToolbar = new QToolBar(this);
    topToolbar->setIconSize(QSize(20,20));

    // construct reset/clear filters button
    resetFiltersAction = topToolbar->addAction("Clear Filters");
    resetFiltersAction->setToolTip("Clear All Filters");
    resetFiltersAction->setVisible(false);

    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setStyleSheet("background: rgba(0,0,0,0);");
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topToolbar->addWidget(stretchWidget);

    centerOnAction = topToolbar->addAction("");
    centerOnAction->setToolTip("Center View Aspect On Selected Item");

    popupAction = topToolbar->addAction("");
    popupAction->setToolTip("View Selected Item In New Window");

    connect(centerOnAction, &QAction::triggered, this, &NotificationDialog::viewSelection);
    connect(popupAction, &QAction::triggered, this, &NotificationDialog::viewSelection);

    /*
     * BOTTOM TOOLBAR
     */
    bottomToolbar = new QToolBar(this);
    bottomToolbar->setIconSize(QSize(20,20));
    //bottomToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    infoLabel = new QLabel(this);
    infoLabel->setFont(QFont(font().family(), font().pointSizeF(), QFont::Normal, true));
    infoAction = bottomToolbar->addWidget(infoLabel);
    infoAction->setVisible(false);

    blinkTimer = new QTimer(this);
    blinkTimer->setTimerType(Qt::PreciseTimer);
    blinkTimer->setInterval(BLINK_INTERVAL);
    if (BLINK_INTERVAL <= 0) {
        blinkTimer->setInterval(BLINK_TIME);
    }
    connect(blinkTimer, &QTimer::timeout, this, &NotificationDialog::intervalTimeout);

    QWidget* stretchWidget2 = new QWidget(this);
    stretchWidget2->setStyleSheet("background: rgba(0,0,0,0);");
    stretchWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomToolbar->addWidget(stretchWidget2);

    sortTimeAction = bottomToolbar->addAction("");
    sortTimeAction->setToolTip("Sort Notifications By Time");
    sortTimeAction->setCheckable(true);
    sortTimeAction->setChecked(true);
    sortTimeAction->setVisible(false);

    sortSeverityAction = bottomToolbar->addAction("");
    sortSeverityAction->setToolTip("Sort Notifications By Severity");
    sortSeverityAction->setCheckable(true);
    sortSeverityAction->setChecked(false);
    sortSeverityAction->setVisible(false);

    //bottomToolbar->addSeparator();

    clearSelectedAction = bottomToolbar->addAction("Clear Selected");
    clearSelectedAction->setToolTip("Clear Selected Items");

    clearVisibleAction = bottomToolbar->addAction("Clear Visible");
    clearVisibleAction->setToolTip("Clear Visible Items");

    connect(clearSelectedAction, &QAction::triggered, this, &NotificationDialog::clearSelected);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);

    QActionGroup* sortGroup = new QActionGroup(this);
    sortGroup->setExclusive(true);
    sortGroup->addAction(sortTimeAction);
    sortGroup->addAction(sortSeverityAction);

    /*
     * FILTERS (LEFT) TOOLBAR
     */
    filtersToolbar = new QToolBar(this);
    filtersToolbar->setOrientation(Qt::Vertical);
    filtersToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    filtersToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QScrollArea* filtersArea = new QScrollArea(this);
    filtersArea->setWidget(filtersToolbar);
    filtersArea->setWidgetResizable(true);

    /*
     * DISPLAY SECTION
     */
    itemsLayout = new QVBoxLayout();
    itemsLayout->setMargin(0);
    itemsLayout->setSpacing(0);
    itemsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    itemsLayout->setSizeConstraint(QLayout::SetMinimumSize);

    QFrame* displayWidget = new QFrame(this);
    displayWidget->setStyleSheet("QFrame{ background: rgba(0,0,0,0); }");
    displayWidget->setLayout(itemsLayout);

    QScrollArea* displayArea = new QScrollArea(this);
    displayArea->setWidget(displayWidget);
    displayArea->setWidgetResizable(true);

    displaySplitter = new QSplitter(this);
    displaySplitter->addWidget(filtersArea);
    displaySplitter->addWidget(displayArea);
    displaySplitter->setStretchFactor(0, 0);
    displaySplitter->setStretchFactor(1, 1);
    displaySplitter->setSizes(QList<int>() << FILTER_DEFAULT_WIDTH << 300);

    mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->addWidget(topToolbar);
    mainLayout->addWidget(displaySplitter, 1);
    mainLayout->addWidget(bottomToolbar);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(mainWidget);

    int minWidth = qMax(topToolbar->sizeHint().width(), bottomToolbar->sizeHint().width());
    int minHeight = topToolbar->sizeHint().height() + bottomToolbar->sizeHint().height() + 75;
    setMinimumSize(minWidth, minHeight);

    setupFilterGroups();
}


/**
 * @brief NotificationDialog::setupFilterGroups
 */
void NotificationDialog::setupFilterGroups()
{
    // setup SEVERITY filter group
    FilterGroup* severityGroup = new FilterGroup("SEVERITY", static_cast<int>(NOTIFICATION_FILTER::SEVERITY), this);
    foreach (NOTIFICATION_SEVERITY severity, getNotificationSeverities()) {
        QString iconName = getSeverityIcon(severity);
        severityGroup->addFilterToolButton(static_cast<int>(severity), getSeverityString(severity), "Icons", iconName);
    }

    // setup CATEGORY filter group
    FilterGroup* categoryGroup = new FilterGroup("CATEGORY", static_cast<int>(NOTIFICATION_FILTER::CATEGORY), this);
    foreach (NOTIFICATION_CATEGORY category, getNotificationCategories()){
        QString iconName = getCategoryIcon(category);
        categoryGroup->addFilterToolButton(static_cast<int>(category), getCategoryString(category), "Icons", iconName);
    }

    // setup TYPE filter group
    FilterGroup* typeGroup = new FilterGroup("SOURCE", static_cast<int>(NOTIFICATION_FILTER::TYPE), this);
    foreach (NOTIFICATION_TYPE type, getNotificationTypes()) {
        QString iconName;
        if (type == NOTIFICATION_TYPE::MODEL) {
            iconName = "dotsInRectangle";
        } else if (type == NOTIFICATION_TYPE::APPLICATION) {
            iconName = "pencil";
        }
        typeGroup->addFilterToolButton(static_cast<int>(type), getTypeString(type), "Icons", iconName);
    }

    // connect filter groups
    connect(severityGroup, &FilterGroup::filtersChanged, this, &NotificationDialog::on_filtersChanged);
    connect(categoryGroup, &FilterGroup::filtersChanged, this, &NotificationDialog::on_filtersChanged);
    connect(typeGroup, &FilterGroup::filtersChanged, this, &NotificationDialog::on_filtersChanged);

    // connect reset filters button to all the filter groups
    connect(resetFiltersAction, &QAction::triggered, severityGroup, &FilterGroup::resetFilters);
    connect(resetFiltersAction, &QAction::triggered, categoryGroup, &FilterGroup::resetFilters);
    connect(resetFiltersAction, &QAction::triggered, typeGroup, &FilterGroup::resetFilters);

    // add filters to the toolbar
    filtersToolbar->addWidget(severityGroup->constructFilterBox());
    filtersToolbar->addWidget(categoryGroup->constructFilterBox());
    filtersToolbar->addWidget(typeGroup->constructFilterBox());

    filterGroups.insert(NOTIFICATION_FILTER::SEVERITY, severityGroup);
    filterGroups.insert(NOTIFICATION_FILTER::CATEGORY, categoryGroup);
    filterGroups.insert(NOTIFICATION_FILTER::TYPE, typeGroup);
}


/**
 * @brief NotificationDialog::updateSelectionBasedButtons
 */
void NotificationDialog::updateSelectionBasedButtons()
{
    bool enable = !selectedItems.isEmpty();
    // TODO - Commented this out to match search centerOn and popup buttons
    //centerOnAction->setEnabled(enable);
    //popupAction->setEnabled(enable);
    clearSelectedAction->setEnabled(enable);
}
