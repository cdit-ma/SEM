#include "notificationdialog.h"
#include "notificationitem.h"
#include "../../theme.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QTime>
#include <QMovie>
#include <QStringBuilder>
#include <QApplication>
#include <QGroupBox>

#define ROLE "ITEM_ROLE"
#define ROLE_VAL "ITEM_ROLE_VALUE"

#define ICON_SIZE 24
#define SHOW_SEPARATOR_MARGIN false

//#define BLINK_INTERVAL 1000
//#define BLINK_TIME 5000
#define BLINK_INTERVAL 0
#define BLINK_TIME 3000

/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent)
    : QWidget(parent)
{
    setupLayout();
    setupBackgroundProcessItems();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(NotificationManager::manager(), &NotificationManager::req_lastNotificationID, this, &NotificationDialog::getLastNotificationID);
    connect(NotificationManager::manager(), &NotificationManager::backgroundProcess, this, &NotificationDialog::backgroundProcess);
    connect(NotificationManager::manager(), &NotificationManager::notificationItemAdded, this, &NotificationDialog::notificationAdded);
    connect(NotificationManager::manager(), &NotificationManager::notificationDeleted, this, &NotificationDialog::notificationDeleted);
    connect(this, &NotificationDialog::deleteNotification, NotificationManager::manager(), &NotificationManager::deleteNotification);
    connect(this, &NotificationDialog::lastNotificationID, NotificationManager::manager(), &NotificationManager::setLastNotificationItem);

    themeChanged();
    initialisePanel();
    updateSelectionBasedButtons();
}


/**
 * @brief NotificationDialog::filterMenuTriggered
 * This is called when an action in the filter menu is triggered.
 * It adds/removes filters from the filter panel.
 * @param action
 */
void NotificationDialog::filterMenuTriggered(QAction* action)
{
    ITEM_ROLES role = (ITEM_ROLES) action->property(ROLE).toInt();
    int index = indexMap.value(role, -1);
    if (index == -1) {
        return;
    }

    ActionGroup* group = actionGroups.value(index, 0);
    QAction* separator = groupSeparators.value(index, 0);
    QList<QAction*> groupActions;

    // show/hide filter group
    bool show = action->isChecked();
    if (group) {
        group->setVisible(show);
        groupActions = group->actions();
    }
    if (separator) {
        separator->setVisible(show);
    }

    // if a filter button/action is hidden, un-check it
    if (!show) {
        foreach (QAction* action, groupActions) {
            if (checkedFilterActions.contains(action)) {
                QToolButton* button = filterButtonHash.value(action, 0);
                if (button) {
                    button->click();
                }
            }
        }
    }
}


/**
 * @brief NotificationDialog::filterToggled
 * This slot is called when any of the filter buttons has been clicked/triggered.
 * @param checked
 */
void NotificationDialog::filterToggled(bool checked)
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {

        // if triggered action is allAction, either clear the filters or re-check the button
        if (action == allAction) {
            if (checked) {
                clearFilters();
            } else {
                /*
                 * For some reason, calling click() changes the checked state of the action
                 * and button as desired, but the visible checked state is unchanged.
                 * However, animateClick() works when they should be sending the same signals.
                 * filterButtonHash[allAction]->click();
                 */
                QToolButton* button = filterButtonHash.value(allAction, 0);
                if (button) {
                    button->animateClick(1);
                }
            }
            return;
        }

        // add/remove action from the checked actions list
        if (checked) {
            checkedFilterActions.append(action);
        } else {
            checkedFilterActions.removeAll(action);
        }

        // if there are no checked actions, send no filters signal and re-check allAction
        // otherwise, if at least one filter action is checked, un-check allAction
        if (checkedFilterActions.isEmpty()) {
            setActionButtonChecked(allAction, true);
            clearFilters();
            return;
        } else if (allAction->isChecked()) {
            setActionButtonChecked(allAction, false);
        }

        // send signal of updated list of the triggered filter to the notification items
        ITEM_ROLES role = (ITEM_ROLES)action->property(ROLE).toInt();
        NOTIFICATION_FILTER filter = getNotificationFilter(role);

        // if the action group doesn't have any checked actions, it is equivalent
        // to the action group having all of its actions checked; send signal to items
        ActionGroup* group = filterGroups.value(role, 0);
        if (group && !group->containsCheckedActions()) {
            clearFilter(filter);
            emit filterCleared(filter);
            return;
        }

        switch (filter) {
        case NF_SEVERITY:
        {
            NOTIFICATION_SEVERITY s = (NOTIFICATION_SEVERITY)action->property(ROLE_VAL).toInt();
            severityCheckedStates[s] = checked;
            emit severityFiltersChanged(severityCheckedStates);
            break;
        }
        case NF_TYPE:
        {
            NOTIFICATION_TYPE2 t = (NOTIFICATION_TYPE2)action->property(ROLE_VAL).toInt();
            typeCheckedStates[t] = checked;
            emit typeFiltersChanged(typeCheckedStates);
            break;
        }
        case NF_CATEGORY:
        {
            NOTIFICATION_CATEGORY c = (NOTIFICATION_CATEGORY)action->property(ROLE_VAL).toInt();
            categoryCheckedStates[c] = checked;
            emit categoryFiltersChanged(categoryCheckedStates);
            break;
        }
        default:
            break;
        }
    }
}


/**
 * @brief NotificationDialog::clearFilter
 * This un-checks all of the actions for the provided filter and updates its corresponding list.
 * @param filter
 */
void NotificationDialog::clearFilter(NOTIFICATION_FILTER filter)
{
    switch (filter) {
    case NF_SEVERITY:
        foreach (NOTIFICATION_SEVERITY s, severityCheckedStates.keys()) {
            severityCheckedStates[s] = false;
        }
        break;
    case NF_TYPE:
        foreach (NOTIFICATION_TYPE2 t, typeCheckedStates.keys()) {
            typeCheckedStates[t] = false;
        }
        break;
    case NF_CATEGORY:
        foreach (NOTIFICATION_CATEGORY c, categoryCheckedStates.keys()) {
            categoryCheckedStates[c] = false;
        }
        break;
    default:
        break;
    }
}


/**
 * @brief NotificationDialog::clearFilters
 * Un-check all checked filter actions/buttons and send a signal to show all notification items.
 */
void NotificationDialog::clearFilters()
{
    // un-check all currently checked filter actions/buttons
    if (!checkedFilterActions.isEmpty()) {
        foreach (QAction* action, checkedFilterActions) {
            setActionButtonChecked(action, false);
        }
        checkedFilterActions.clear();
    }

    // reset checked-states lists
    foreach (NOTIFICATION_FILTER filter, NotificationManager::getNotificationFilters()) {
        clearFilter(filter);
    }

    // send signal to show all notification items
    emit filtersCleared();
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
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
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
                                  "QToolBar::separator {"
                                  "margin: 2px 0px;"
                                  "height: 4px;"
                                  "background:" + theme->getDisabledBackgroundColorHex() + ";"
                                  "}"
                                  "QToolButton {"
                                  "padding: 5px 10px 5px 3px;"
                                  "border-radius:" + theme->getSharpCornerRadius() + ";"
                                  "}");

    filtersMenu->setStyleSheet("QMenu::item{ padding: 5px 15px 5px 25px; }");
    filtersButton->setStyleSheet("QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}"
                                 "QToolButton:!hover{ background: rgba(0,0,0,0); }"
                                 "QToolButton:pressed{ background:" + theme->getPressedColorHex() + ";}"
                                 "QToolButton::menu-indicator{ subcontrol-position: right center; }");

    sortTimeAction->setIcon(theme->getIcon("Actions", "Clock"));
    sortSeverityAction->setIcon(theme->getIcon("Actions", "Sort"));
    centerOnAction->setIcon(theme->getIcon("Actions", "Crosshair"));
    popupAction->setIcon(theme->getIcon("Actions", "Popup"));
    clearSelectedAction->setIcon(theme->getIcon("Actions", "Delete"));
    clearVisibleAction->setIcon(theme->getIcon("Actions", "Clear"));

    displaySplitter->setStyleSheet(theme->getSplitterStyleSheet());
    displaySeparator->setStyleSheet("color:" + theme->getDisabledBackgroundColorHex() + ";");

    foreach (QToolButton* button, filterButtonHash.values()) {
        QString iconPath = button->property("iconPath").toString();
        QString iconName = button->property("iconName").toString();
        button->setIcon(theme->getIcon(iconPath, iconName));
    }
}


/**
 * @brief NotificationDialog::updateSelection
 * @param item - the notification item that was clicked
 * @param selected - the item's current selected state
 * @param controlDown - control key's down state
 */
void NotificationDialog::updateSelection(NotificationItem* item, bool selected, bool controlDown)
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
        if (!item->isVisible()) {
            continue;
        }
        visibleItems.append(item);
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
        case NF_SEVERITY:
        {
            if (item->getSeverity() == (NOTIFICATION_SEVERITY)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        case NF_TYPE:
        {
            if (item->getType() == (NOTIFICATION_TYPE2)filterVal) {
                itemsToDelete.append(item);
            }
            break;
        }
        case NF_CATEGORY:
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
    NOTIFICATION_SEVERITY severity = obj->severity();

    itemsLayout->insertWidget(0, item);
    notificationItems[obj->ID()] = item;
    severityItemsCount[severity]++;
    updateSeverityAction(severity);

    connect(item, &NotificationItem::itemClicked, this, &NotificationDialog::updateSelection);
    connect(this, &NotificationDialog::filtersCleared, item, &NotificationItem::showItem);
    connect(this, &NotificationDialog::filterCleared, item, &NotificationItem::filterCleared);
    connect(this, &NotificationDialog::severityFiltersChanged, item, &NotificationItem::severityFilterToggled);
    connect(this, &NotificationDialog::typeFiltersChanged, item, &NotificationItem::typeFilterToggled);
    connect(this, &NotificationDialog::categoryFiltersChanged, item, &NotificationItem::categoryFilterToggled);

    // update the item's visibility depending on the currently checked filters
    if (!checkedFilterActions.isEmpty()) {
        emit severityFiltersChanged(severityCheckedStates);
        emit typeFiltersChanged(typeCheckedStates);
        emit categoryFiltersChanged(categoryCheckedStates);
    }
}


/**
 * @brief NotificationDialog::notificationDeleted
 * Delete notification item with the provided ID from the hash and the items layout.
 * Send a signal to the notification toolbar to update the item's severity's count.
 * @param ID
 */
void NotificationDialog::notificationDeleted(int ID)
{
    if (notificationItems.contains(ID)) {
        NotificationItem* item = notificationItems.take(ID);
        NOTIFICATION_SEVERITY severity = item->getSeverity();
        if (severityItemsCount.contains(severity)) {
            severityItemsCount[severity]--;
        }
        selectedItems.removeAll(item);
        itemsLayout->removeWidget(item);
        updateSelectionBasedButtons();
        updateSeverityAction(severity);
        delete item;
    }
}


/**
 * @brief NotificationDialog::clearAll
 * NOTE: This function is the only one that doesn't use notificationDeleted to delete items.
 */
void NotificationDialog::clearAll()
{
    // remove widgets from the items layout
    QLayoutItem* child;
    while ((child = itemsLayout->takeAt(0)) != 0) {
        delete child;
    }

    // hide background process items
    foreach (BACKGROUND_PROCESS process, backgroundProcesses.keys()) {
        backgroundProcess(false, process);
    }

    // clear the severity items count; the values here are sent to the notification toolbar
    foreach (NOTIFICATION_SEVERITY severity, severityItemsCount.keys()) {
        severityItemsCount[severity] = 0;
    }

    notificationItems.clear();
    selectedItems.clear();
    visibleProcessCount = 0;

    // reset checked filter buttons and checked filter lists
    clearFilters();

    updateSeverityActions(NotificationManager::getNotificationSeverities());
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
    //resetDialog();

    QList<NotificationObject*> notifications = NotificationManager::getNotificationItems();
    if (!notifications.isEmpty()) {
        foreach (NotificationObject* item, notifications) {
            notificationAdded(item);
        }
        NotificationManager::manager()->notificationAlert();
        NotificationManager::manager()->showLastNotification();
    } else {
        NotificationManager::manager()->notificationSeen();
    }
}


/**
 * @brief NotificationDialog::getNotificationFilter
 * @param role
 * @return
 */
NOTIFICATION_FILTER NotificationDialog::getNotificationFilter(NotificationDialog::ITEM_ROLES role)
{
    switch (role) {
    case IR_SEVERITY:
        return NF_SEVERITY;
    case IR_TYPE:
        return NF_TYPE;
    case IR_CATEGORY:
        return NF_CATEGORY;
    default:
        return NF_NOFILTER;
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
 * @brief NotificationDialog::backgroundProcess
 * @param inProgress
 * @param process
 */
void NotificationDialog::backgroundProcess(bool inProgress, BACKGROUND_PROCESS process)
{
    QFrame* processItem = backgroundProcesses.value(process, 0);
    if (!processItem) {
        qWarning() << "NotificationDialog::backgroundProcess - Background process has not been setup.";
        return;
    }
    /*
    if (inProgress) {
        if (!processItem->isVisible()) {
            visibleProcessCount++;
            processItem->show();
            if (!displayedSeparatorFrame->isVisible()) {
                displayedSeparatorFrame->show();
            }
        }
    } else {
        visibleProcessCount--;
        processItem->hide();
        displayedSeparatorFrame->setVisible(visibleProcessCount > 0);
    }
    */
    if (inProgress) {
        visibleProcessCount++;
    } else {
        visibleProcessCount--;
    }
    processItem->setVisible(inProgress);
    displayedSeparatorFrame->setVisible(visibleProcessCount > 0);
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
        if (severity != NS_ERROR) {
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

void NotificationDialog::test()
{
    /*
     * Just leaving this here for a reminder
     */
    QGroupBox* box = new QGroupBox("TEST BOX", this);
    box->setCheckable(true);
    box->setStyleSheet("QGroupBox{ margin-top: 6px; border: none; border-top: 2px solid red; }"
                       "QGroupBox::title{ subcontrol-origin: margin; subcontrol-position: top center; padding: 0 3px; }"
                       "QGroupBox::indicator{ width: 13px; height: 13px; subcontrol-position: right; }"
                       "QGroupBox::indicator:checked{ image: url(:/Actions/Arrow_Up); }"
                       "QGroupBox::indicator:unchecked{ image: url(:/Actions/Arrow_Down); }");

    QPushButton* b = new QPushButton("Button1", this);
    b->setCheckable(true);
    b->setChecked(true);
    b->setStyleSheet("QPushButton:checked{background: red;}QPushButton{ background:yellow; }");
    QPushButton* b2 = new QPushButton("Button2", this);
    b2->setCheckable(true);

    QVBoxLayout* bl = new QVBoxLayout(box);
    bl->addWidget(b);
    bl->addWidget(b2);
    connect(box, SIGNAL(toggled(bool)), b, SLOT(setVisible(bool)));
    connect(box, SIGNAL(toggled(bool)), b2, SLOT(setVisible(bool)));

    QDialog* dialog = new QDialog(this);
    QVBoxLayout* vl = new QVBoxLayout(dialog);
    vl->addWidget(box);
    dialog->exec();
}


/**
 * @brief NotificationDialog::updateSeverityActions
 * @param severities
 */
void NotificationDialog::updateSeverityActions(QList<NOTIFICATION_SEVERITY> severities)
{
    foreach (NOTIFICATION_SEVERITY severity, severities) {
        updateSeverityAction(severity);
    }
}


/**
 * @brief NotificationDialog::updateSeverityAction
 * This updates the text (displaying the items count) in the button for the particular severity.
 * @param severity
 */
void NotificationDialog::updateSeverityAction(NOTIFICATION_SEVERITY severity)
{
    emit updateSeverityCount(severity, severityItemsCount.value(severity, 0));
}


/**
 * @brief NotificationDialog::setupLayout
 */
void NotificationDialog::setupLayout()
{
    // setup and populate the filters menu
    filtersMenu = new QMenu(this);
    filtersMenu->addAction("Severity")->setProperty(ROLE, IR_SEVERITY);
    filtersMenu->addAction("Category")->setProperty(ROLE, IR_CATEGORY);
    filtersMenu->addAction("Scope")->setProperty(ROLE, IR_TYPE);
    connect(filtersMenu, &QMenu::triggered, this, &NotificationDialog::filterMenuTriggered);

    // initially check all of the filter groups in the menu
    foreach (QAction* action, filtersMenu->actions()) {
        action->setCheckable(true);
        action->setChecked(true);
    }

    filtersButton = new QToolButton(this);
    filtersButton->setPopupMode(QToolButton::InstantPopup);
    filtersButton->setFont(QFont(font().family(), 10));
    filtersButton->setText("Filters");
    filtersButton->setMenu(filtersMenu);

    /*
     * TOP TOOLBAR
     */
    topToolbar = new QToolBar(this);
    topToolbar->setIconSize(QSize(20,20));
    topToolbar->addWidget(filtersButton);

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
    filtersToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filtersToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    filtersToolbar->setIconSize(QSize(18,18));
    //filtersToolbar->setIconSize(QSize(20,20));

    QScrollArea* filtersArea = new QScrollArea(this);
    filtersArea->setWidget(filtersToolbar);
    filtersArea->setWidgetResizable(true);

    allAction = constructFilterButtonAction((ITEM_ROLES)-1, -1, "All", "Actions", "Menu", false);
    setActionButtonChecked(allAction, true);

    // setup the SEVERITY, TYPE, and CATEGORY filter actions/buttons in that order
    NotificationManager* manager = NotificationManager::manager();
    foreach (NOTIFICATION_SEVERITY severity, manager->getNotificationSeverities()) {
        QString iconName = manager->getSeverityString(severity);
        constructFilterButtonAction(IR_SEVERITY, severity, manager->getSeverityString(severity), "Actions", iconName);
        severityCheckedStates[severity] = false;
    }
    foreach (NOTIFICATION_CATEGORY category, manager->getNotificationCategories()) {
        //constructFilterButtonAction(IR_CATEGORY, category, manager->getCategoryString(category), "Data", "severity");
        constructFilterButtonAction(IR_CATEGORY, category, manager->getCategoryString(category), "Actions", "Splitter");
        categoryCheckedStates[category] = false;
    }
    foreach (NOTIFICATION_TYPE2 type, manager->getNotificationTypes()) {
        QString iconName;
        if (type == NT_MODEL) {
            iconName = "Model";
        } else if (type == NT_APPLICATION) {
            iconName = "Rename";
        }
        constructFilterButtonAction(IR_TYPE, type, manager->getTypeString(type), "Actions", iconName);
        typeCheckedStates[type] = false;
    }

    /*
     * DISPLAY SECTION
     */
    processLayout = new QVBoxLayout();
    processLayout->setMargin(0);
    processLayout->setSpacing(0);

    displaySeparator = new QFrame(this);
    displaySeparator->setLineWidth(5);
    displaySeparator->setFixedHeight(4);
    displaySeparator->setFrameShape(QFrame::HLine);

    if (SHOW_SEPARATOR_MARGIN) {
        QFrame* displaySeparatorFrame = new QFrame(this);
        displaySeparatorFrame->setFixedHeight(15);
        QVBoxLayout* separatorLayout = new QVBoxLayout(displaySeparatorFrame);
        separatorLayout->setContentsMargins(0, 2, 0, 0);
        separatorLayout->setMargin(0);
        separatorLayout->addWidget(displaySeparator);
        displayedSeparatorFrame = displaySeparatorFrame;
    } else {
        displayedSeparatorFrame = displaySeparator;
    }

    displayedSeparatorFrame->hide();

    itemsLayout = new QVBoxLayout();
    itemsLayout->setMargin(0);
    itemsLayout->setSpacing(0);
    itemsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    itemsLayout->setSizeConstraint(QLayout::SetMinimumSize);

    QFrame* displayWidget = new QFrame(this);
    displayWidget->setStyleSheet("QFrame{ background: rgba(0,0,0,0); }");

    QVBoxLayout* displayLayout = new QVBoxLayout(displayWidget);
    displayLayout->setMargin(0);
    displayLayout->setSpacing(0);
    displayLayout->addLayout(processLayout);
    displayLayout->addWidget(displayedSeparatorFrame);
    displayLayout->addLayout(itemsLayout, 1);

    QScrollArea* displayArea = new QScrollArea(this);
    displayArea->setWidget(displayWidget);
    displayArea->setWidgetResizable(true);

    displaySplitter = new QSplitter(this);
    displaySplitter->addWidget(filtersArea);
    displaySplitter->addWidget(displayArea);
    displaySplitter->setStretchFactor(0, 0);
    displaySplitter->setStretchFactor(1, 1);
    displaySplitter->setSizes(QList<int>() << 135 << 215);

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

    //setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);

    int minWidth = qMax(topToolbar->sizeHint().width(), bottomToolbar->sizeHint().width());
    int minHeight = topToolbar->sizeHint().height() + bottomToolbar->sizeHint().height() + 75;
    setMinimumSize(minWidth, minHeight);

    // initially hide the source filters
    filtersMenu->actions().last()->trigger();
}


/**
 * @brief NotificationDialog::setupBackgroundProcessItems
 */
void NotificationDialog::setupBackgroundProcessItems()
{
    foreach (BACKGROUND_PROCESS process, NotificationManager::getBackgroundProcesses()) {
        QString description;
        switch (process) {
        case BP_VALIDATION:
            description = "Validating Model ...";
            break;
        case BP_IMPORT_JENKINS:
            description = "Importing Jenkins Nodes ...";
            break;
        default:
            description = "Background Process In Progress ...";
            break;
        }

        QMovie* loadingGif = new QMovie(this);
        loadingGif->setFileName(":/Actions/Waiting");
        loadingGif->start();

        //QLabel* textLabel = new QLabel("<i>" + description + "</i>", this);
        QLabel* textLabel = new QLabel(description, this);
        QLabel* iconLabel = new QLabel(this);
        iconLabel->setMovie(loadingGif);

        QFrame* frame = new QFrame(this);
        frame->setStyleSheet("border: 1px 0px;");
        frame->hide();

        QHBoxLayout* layout = new QHBoxLayout(frame);
        layout->addWidget(iconLabel);
        layout->addSpacerItem(new QSpacerItem(5,0));
        layout->addWidget(textLabel, 1);

        processLayout->addWidget(frame);
        backgroundProcesses[process] = frame;
    }

    visibleProcessCount = 0;
}


/**
 * @brief NotificationDialog::constructFilterButtonAction
 * @param role
 * @param roleVal
 * @param label
 * @param iconPath
 * @param iconName
 * @param addToGroup
 * @return
 */
QAction* NotificationDialog::constructFilterButtonAction(NotificationDialog::ITEM_ROLES role, int roleVal, QString label, QString iconPath, QString iconName, bool addToGroup)
{
    ActionGroup* group = 0;

    if (addToGroup) {

        // check if there is already an action group for the provided role
        int index = indexMap.value(role, -1);
        group = actionGroups.value(index, 0);

        if (!group) {
            // construct new group and add to hash
            group = new ActionGroup(this);
            group->setExclusive(false);
            filterGroups[role] = group;

            // construct separator action for new group
            QAction* separator = filtersToolbar->addSeparator();
            index = indexMap.size();
            indexMap[role] = index;
            actionGroups.insert(index, group);
            groupSeparators.insert(index, separator);
        }
    }

    // set default icon
    if (iconPath.isEmpty() || iconName.isEmpty()) {
        iconPath = "Actions";
        iconName = "Help";
    }

    QToolButton* button = new QToolButton(this);
    button->setText(label);
    button->setProperty("iconPath", iconPath);
    button->setProperty("iconName", iconName);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    button->setCheckable(true);
    button->setChecked(false);

    QAction* action = filtersToolbar->addWidget(button);
    action->setCheckable(true);
    action->setChecked(false);
    action->setProperty(ROLE, role);
    action->setProperty(ROLE_VAL, roleVal);

    if (group) {
        group->addAction(action);
    }

    filterButtonHash[action] = button;

    connect(button, SIGNAL(clicked(bool)), action, SLOT(trigger()));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(filterToggled(bool)));
    connect(action, SIGNAL(triggered(bool)), button, SLOT(setChecked(bool)));

    return action;
}


/**
 * @brief NotificationDialog::setActionButtonChecked
 * This method sets the action's and its corresponding tool button's checked state to checked.
 * @param action
 * @param checked
 */
void NotificationDialog::setActionButtonChecked(QAction* action, bool checked)
{
    QToolButton* button = filterButtonHash.value(action, 0);
    if (action && button) {
        action->setChecked(checked);
        button->setChecked(checked);
    }
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
