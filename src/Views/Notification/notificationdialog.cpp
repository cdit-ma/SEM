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

#define ROLE "ITEM_ROLE"
#define ROLE_VAL "ITEM_ROLE_VALUE"
#define ICON_SIZE 24
#define SHOW_SEPARATOR_MARGIN false

/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupLayout();
    setupBackgroundProcessItems();
    setWindowTitle("Notifications");

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(NotificationManager::manager(), &NotificationManager::clearNotifications, this, &NotificationDialog::clearNotifications);
    connect(NotificationManager::manager(), &NotificationManager::showNotificationDialog, this, &NotificationDialog::showDialog);
    connect(NotificationManager::manager(), &NotificationManager::backgroundProcess, this, &NotificationDialog::backgroundProcess);
    connect(NotificationManager::manager(), &NotificationManager::notificationItemAdded, this, &NotificationDialog::notificationItemAdded);
    connect(NotificationManager::manager(), &NotificationManager::req_lastNotificationID, this, &NotificationDialog::getLastNotificationID);
    connect(this, &NotificationDialog::lastNotificationID, NotificationManager::manager(), &NotificationManager::setLastNotificationItem);
    connect(this, &NotificationDialog::itemDeleted, NotificationManager::manager(), &NotificationManager::deleteNotification);

    themeChanged();
    initialiseDialog();
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

    // show/hide filter group
    ActionGroup* group = actionGroups.value(index, 0);
    bool show = action->isChecked();
    if (group) {
        group->setVisible(show);
    }
    QAction* separator = groupSeparators.value(index, 0);
    if (separator) {
        separator->setVisible(show);
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
                filterButtonHash[allAction]->animateClick(1);
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
    setStyleSheet("QListWidget{ border: 1px solid " + theme->getDisabledBackgroundColorHex() + "; }"
                  + theme->getAltAbstractItemViewStyleSheet()
                  + theme->getDialogStyleSheet()
                  + "QScrollArea {"
                    "background:" + theme->getBackgroundColorHex() + ";"
                    "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                    "}"
                  + "QLabel{ background: rgba(0,0,0,0); color:" + theme->getTextColorHex()+ ";}");

    iconOnlyToolbar->setStyleSheet(theme->getToolBarStyleSheet());
    filtersToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                                  "QToolBar::separator {"
                                  "margin: 2px 0px;"
                                  "height: 4px;"
                                  "background:" + theme->getDisabledBackgroundColorHex() + ";"
                                  "}"
                                  "QToolButton {"
                                  "padding: 5px;"
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
        //qDebug() << "button[" << button->text() << "]: " << iconPath << "," << iconName;
    }
}


/**
 * @brief NotificationDialog::updateSelection
 * @param item
 * @param selected
 * @param controlDown
 */
void NotificationDialog::updateSelection(NotificationItem* item, bool selected, bool controlDown)
{
    if (!item) {
        return;
    }

    if (selected) {
        if (controlDown) {
            item->setSelected(false);
            selectedItems.removeAll(item);
        } else {
            clearSelection();
            item->setSelected(true);
            selectedItems.append(item);
        }
    } else {
        item->setSelected(true);
        if (!controlDown) {
           clearSelection();
        }
        selectedItems.append(item);
    }

    bool selectionExists = !selectedItems.isEmpty();
    bool currentState = clearSelectedAction->isEnabled();
    if (currentState != selectionExists) {
        clearSelectedAction->setEnabled(selectionExists);
        centerOnAction->setEnabled(selectionExists);
        popupAction->setEnabled(selectionExists);
    }
}


/**
 * @brief NotificationDialog::toggleVisibility
 * Toggle this dialog's visibility.
 * TODO - If already visible, find a way to keep dialog visible and raise it to the top.
 */
void NotificationDialog::toggleVisibility()
{
    /*
    if (isVisible()) {
        if (QApplication::activeWindow() != this) {
            raise();
            return;
        }
    }
    */

    setVisible(!isVisible());

    // if the dialog was just made visible, send this signal to mark new notifications as seen
    if (isVisible()) {
        emit mouseEntered();
    }
}


/**
 * @brief NotificationDialog::showDialog
 * Force show and raise this dialog.
 */
void NotificationDialog::showDialog()
{
    show();
    raise();
}


/**
 * @brief NotificationDialog::resetDialog
 */
void NotificationDialog::resetDialog()
{
    hide();
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
            NotificationItem* item = dynamic_cast<NotificationItem*>(topItem);
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
    QList<NOTIFICATION_SEVERITY> removedSeverities;

    // delete selected items
    while (!selectedItems.isEmpty()) {
        NotificationItem* item = selectedItems.takeFirst();
        NOTIFICATION_SEVERITY severity = (NOTIFICATION_SEVERITY) item->getSeverity();
        if (severity != NS_ERROR) {
            if (!removedSeverities.contains(severity)) {
                removedSeverities.append(severity);
            }
            removeItem(item->getID());
        }
    }
    updateSeverityActions(removedSeverities);
}


/**
 * @brief NotificationDialog::clearVisible
 */
void NotificationDialog::clearVisible()
{
    QList<NotificationItem*> visibleItems;
    QList<NOTIFICATION_SEVERITY> removedSeverities;

    foreach (NotificationItem* item, notificationItems) {
        NOTIFICATION_SEVERITY severity = item->getSeverity();
        if (severity != NS_ERROR) {
            visibleItems.append(item);
            if (!removedSeverities.contains(severity)) {
                removedSeverities.append(severity);
            }
        }
    }

    // delete visible items
    while (!visibleItems.isEmpty()) {
        removeItem(visibleItems.takeFirst()->getID());
    }
    updateSeverityActions(removedSeverities);
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

    QList<NOTIFICATION_SEVERITY> removedSeverities;
    foreach (NotificationItem* item, itemsToDelete) {
        NOTIFICATION_SEVERITY severity = item->getSeverity();
        if (!removedSeverities.contains(severity)) {
            removedSeverities.append(severity);
        }
        removeItem(item->getID());
    }
    updateSeverityActions(removedSeverities);
}


/**
 * @brief NotificationDialog::notificationItemAdded
 * @param item
 */
void NotificationDialog::notificationItemAdded(NotificationObject* obj)
{
    if (!obj) {
        return;
    }

    NotificationItem* item = new NotificationItem(obj, this);

    itemsLayout->insertWidget(0, item);
    notificationItems[obj->ID()] = item;
    severityItemsCount[obj->severity()]++;

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
 * @brief NotificationDialog::notificationItemDeleted
 * @param ID
 * @param severity
 */
void NotificationDialog::notificationItemDeleted(int ID, NOTIFICATION_SEVERITY severity)
{
    removeItem(ID);
    updateSeverityAction(severity);
}


/**
 * @brief NotificationDialog::clearAll
 */
void NotificationDialog::clearAll()
{
    // remove widgets from the items and process layouts
    QLayoutItem* child;
    while ((child = itemsLayout->takeAt(0)) != 0) {
        delete child;
    }
    while ((child = processLayout->takeAt(0)) != 0) {
        delete child;
    }

    // clear the severity items count; the values here are sent to the notification toolbar
    foreach (NOTIFICATION_SEVERITY severity, severityItemsCount.keys()) {
        severityItemsCount[severity] = 0;
    }

    notificationItems.clear();
    backgroundProcesses.clear();
    selectedItems.clear();

    visibleProcessCount = 0;
    updateVisibilityCount(0, true);
    updateSeverityActions(NotificationManager::getNotificationSeverities());

    // reset checked filter buttons and checked filter lists
    clearFilters();
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
 * @brief NotificationDialog::initialiseDialog
 * Construct items for notifications that were received before this dialog was constructed.
 * Show highlight alert and toast for the last notification item in the list.
 */
void NotificationDialog::initialiseDialog()
{
    resetDialog();

    QList<NotificationObject*> notifications = NotificationManager::getNotificationItems();
    if (!notifications.isEmpty()) {
        foreach (NotificationObject* item, notifications) {
            notificationItemAdded(item);
        }
        NotificationManager::manager()->notificationAlert();
        NotificationManager::manager()->showLastNotification();
    } else {
        NotificationManager::manager()->notificationSeen();
    }
}


/**
 * @brief NotificationDialog::removeItem
 * Remove notification item with the provided ID from the hash and the items layout.
 * @param ID
 */
void NotificationDialog::removeItem(int ID)
{
    if (notificationItems.contains(ID)) {
        NotificationItem* item = notificationItems.take(ID);
        NOTIFICATION_SEVERITY severity = item->getSeverity();
        if (item->isVisible()) {
            updateVisibilityCount(-1);
        }
        if (severityItemsCount.contains(severity)) {
            severityItemsCount[severity]--;
        }
        itemsLayout->removeWidget(item);
        delete item;
        emit itemDeleted(ID);
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
    QDialog::enterEvent(event);
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
    if (inProgress != processItem->isVisible()) {
        processItem->setVisible(inProgress);
        if (inProgress) {
            visibleProcessCount++;
        } else {
            visibleProcessCount--;
        }
        bool showSeparator = visibleProcessCount > 0;
        if (showSeparator != displayedSeparatorFrame->isVisible()) {
            displayedSeparatorFrame->setVisible(showSeparator);
        }
    }
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
    QWidget* w = new QWidget(this);
    w->setVisible(false);

    QVBoxLayout* mainLayout = new QVBoxLayout(w);
    //QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(DIALOG_SPACING);

    topToolbar = new QToolBar(this);
    topToolbar->setIconSize(QSize(20,20));

    /*
    centerOnAction = topToolbar->addAction("");
    centerOnAction->setToolTip("Center View Aspect On Selected Item");
    centerOnAction->setEnabled(false);

    popupAction = topToolbar->addAction("");
    popupAction->setToolTip("View Selected Item In New Window");
    popupAction->setEnabled(false);
    */

    iconOnlyToolbar = new QToolBar(this);
    iconOnlyToolbar->setIconSize(QSize(20,20));

    sortTimeAction = iconOnlyToolbar->addAction("");
    sortTimeAction->setToolTip("Sort Notifications By Time");
    sortTimeAction->setCheckable(true);
    sortTimeAction->setChecked(true);
    sortTimeAction->setVisible(false);

    sortSeverityAction = iconOnlyToolbar->addAction("");
    sortSeverityAction->setToolTip("Sort Notifications By Severity");
    sortSeverityAction->setCheckable(true);
    sortSeverityAction->setChecked(false);
    sortSeverityAction->setVisible(false);

    //iconOnlyToolbar->addSeparator();

    clearSelectedAction = iconOnlyToolbar->addAction("Clear Selected");
    clearSelectedAction->setToolTip("Clear Selected Items");
    clearSelectedAction->setEnabled(false);

    clearVisibleAction = iconOnlyToolbar->addAction("Clear Visible");
    clearVisibleAction->setToolTip("Clear Visible Items");
    clearVisibleAction->setEnabled(false);

    QActionGroup* sortGroup = new QActionGroup(this);
    sortGroup->setExclusive(true);
    sortGroup->addAction(sortTimeAction);
    sortGroup->addAction(sortSeverityAction);

    QWidget* holderWidget = new QWidget(this);
    holderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(holderWidget);
    topLayout->addWidget(topToolbar);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(iconOnlyToolbar);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    //connect(centerOnAction, &QAction::triggered, this, &NotificationDialog::viewSelection);
    //connect(popupAction, &QAction::triggered, this, &NotificationDialog::viewSelection);
    connect(clearSelectedAction, &QAction::triggered, this, &NotificationDialog::clearSelected);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);

    setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);
    
    // setup and populate the filters menu
    filtersMenu = new QMenu(this);
    filtersMenu->addAction("Severity")->setProperty(ROLE, IR_SEVERITY);
    filtersMenu->addAction("Source")->setProperty(ROLE, IR_TYPE);
    filtersMenu->addAction("Category")->setProperty(ROLE, IR_CATEGORY);
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

    topButtonsToolbar = new QToolBar(this);
    topButtonsToolbar->setIconSize(QSize(20,20));
    topButtonsToolbar->addWidget(filtersButton);

    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setStyleSheet("background: rgba(0,0,0,0);");
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topButtonsToolbar->addWidget(stretchWidget);

    centerOnAction = topButtonsToolbar->addAction("");
    centerOnAction->setToolTip("Center View Aspect On Selected Item");
    centerOnAction->setEnabled(false);

    popupAction = topButtonsToolbar->addAction("");
    popupAction->setToolTip("View Selected Item In New Window");
    popupAction->setEnabled(false);

    filtersToolbar = new QToolBar(this);
    filtersToolbar->setOrientation(Qt::Vertical);
    filtersToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filtersToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    filtersToolbar->setIconSize(QSize(20,20));

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
    foreach (NOTIFICATION_CATEGORY category, manager->getNotificationCategories()) {
        constructFilterButtonAction(IR_CATEGORY, category, manager->getCategoryString(category));
        categoryCheckedStates[category] = false;
    }

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
    displaySplitter->setSizes(QList<int>() << 150 << 200);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setMargin(DIALOG_MARGIN);
    vLayout->setSpacing(DIALOG_SPACING);
    vLayout->addWidget(topButtonsToolbar);
    vLayout->addWidget(displaySplitter, 1);

    // initially hide the category filters
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
 * @brief NotificationDialog::updateVisibilityCount
 * This enables/disables the clearVisible button.
 * @param val
 * @param set
 */
void NotificationDialog::updateVisibilityCount(int val, bool set)
{
    if (set) {
        visibleCount = val;
    } else {
        visibleCount += val;
    }
    bool enableAction = visibleCount > 0;
    if (enableAction != clearVisibleAction->isEnabled()) {
        clearVisibleAction->setEnabled(enableAction);
    }
}
