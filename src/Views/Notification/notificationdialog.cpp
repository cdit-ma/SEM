#include "notificationdialog.h"
#include "notificationitem.h"
#include "../../theme.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QTime>
#include <QStringBuilder>
#include <QApplication>

#define ICON_SIZE 24
#define ROLE "ITEM_ROLE"
#define ROLE_VAL "ITEM_ROLE_VALUE"

/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent) :
    QDialog(parent)
{
    QSignalMapper* severityActionMapper = new QSignalMapper(this);
    foreach (NOTIFICATION_SEVERITY severity, NotificationManager::getNotificationSeverities()) {
        QAction* action = new QAction(this);
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, &QAction::toggled, severityActionMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        severityActionMapper->setMapping(action, severity);
        severityActionHash.insert(severity, action);
        updateSeverityAction(severity);
    }

    setupLayout();
    setupLayout2();
    setWindowTitle("Notifications");

    connect(severityActionMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped),this, &NotificationDialog::severityActionToggled);
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    connect(NotificationManager::manager(), &NotificationManager::clearNotifications, this, &NotificationDialog::showDialog);
    connect(NotificationManager::manager(), &NotificationManager::showNotificationDialog, this, &NotificationDialog::showDialog);
    connect(NotificationManager::manager(), &NotificationManager::notificationItemAdded, this, &NotificationDialog::notificationItemAdded);
    connect(NotificationManager::manager(), &NotificationManager::req_lastNotificationID, this, &NotificationDialog::getLastNotificationID);
    connect(this, &NotificationDialog::lastNotificationID, NotificationManager::manager(), &NotificationManager::setLastNotificationItem);
    connect(this, &NotificationDialog::itemDeleted, NotificationManager::manager(), &NotificationManager::deleteNotification);

    themeChanged();
    updateVisibilityCount(0, true);
}


/**
 * @brief NotificationDialog::severityActionToggled
 * @param actionSeverity
 */
void NotificationDialog::severityActionToggled(int actionSeverity)
{
    NOTIFICATION_SEVERITY severity = (NOTIFICATION_SEVERITY) actionSeverity;
    QAction* action = severityActionHash.value(severity, 0);
    bool clearSelection = false;
    if (action) {
        bool visible = action->isChecked();
        foreach (QListWidgetItem* item, notificationHash.values(severity)) {
            int row = listWidget->row(item);
            listWidget->setRowHidden(row, !visible);
            if (visible) {
                updateVisibilityCount(1);
            } else {
                updateVisibilityCount(-1);
            }
            clearSelection = clearSelection || item->isSelected();
        }
    }
    if (clearSelection) {
        listWidget->clearSelection();
    }
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
    QActionGroup* group = actionGroups.value(index, 0);
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
            checkedActions.append(action);
        } else {
            checkedActions.removeAll(action);
        }

        // if there are no checked actions, send no filters signal and re-check allAction
        // otherwise, if at least one filter action is checked, un-check allAction
        if (checkedActions.isEmpty()) {
            setActionButtonChecked(allAction, true);
            emit filtersCleared();
            return;
        } else if (allAction->isChecked()) {
            setActionButtonChecked(allAction, false);
        }

        // send signal of updated list of the triggered filter to the notification items
        NOTIFICATION_FILTER f = getNotificationFilter((ITEM_ROLES)action->property(ROLE).toInt());
        switch (f) {
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
 * @brief NotificationDialog::clearFilters
 */
void NotificationDialog::clearFilters()
{
    if (!checkedActions.isEmpty()) {
        foreach (QAction* action, checkedActions) {
            setActionButtonChecked(action, false);
        }
        checkedActions.clear();
        emit filtersCleared();
    }
}


/**
 * @brief NotificationDialog::viewSelection
 */
void NotificationDialog::viewSelection()
{
    int numSelectedItems = listWidget->selectedItems().count();
    if (numSelectedItems != 1) {
        return;
    }

    QListWidgetItem* selectedItem = listWidget->selectedItems().at(0);
    int eID = selectedItem->data(IR_ENTITYID).toInt();
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

    /*
    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                              "QToolButton{ padding: 2px; border-radius:" + theme->getSharpCornerRadius() + "; color:" + theme->getTextColorHex() + ";}"
                              "QToolButton::checked{ background:" + theme->getPressedColorHex() + ";color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}"
                              "QToolButton:hover{ background:" + theme->getHighlightColorHex() + "; color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}");
                              */

    bottomToolbar->setStyleSheet(theme->getToolBarStyleSheet());
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

    foreach (QToolButton* button, filterButtonHash.values()) {
        QString iconPath = button->property("iconPath").toString();
        QString iconName = button->property("iconName").toString();
        button->setIcon(theme->getIcon(iconPath, iconName));
    }

    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        if (item) {
            QString iconPath = item->data(IR_ICONPATH).toString();
            QString iconName = item->data(IR_ICONNAME).toString();
            item->setIcon(theme->getIcon(iconPath, iconName));
        }
    }

    foreach (NOTIFICATION_SEVERITY severity, NotificationManager::getNotificationSeverities()) {
        QAction* action = severityActionHash.value(severity, 0);
        if (action) {
             QPair<QString, QString> iconPath = getActionIcon(severity);
             action->setIcon(theme->getIcon(iconPath));
        }
    }
}


/**
 * @brief NotificationDialog::listSelectionChanged
 */
void NotificationDialog::listSelectionChanged()
{
    bool csEnabled = clearSelectedAction->isEnabled();
    bool selectionExists = !listWidget->selectedItems().isEmpty();
    if (csEnabled != selectionExists) {
        clearSelectedAction->setEnabled(selectionExists);
        centerOnAction->setEnabled(selectionExists);
        popupAction->setEnabled(selectionExists);
    }
}


/**
 * @brief NotificationDialog::toggleVisibility
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
 */
void NotificationDialog::getLastNotificationID()
{
    if (listWidget->count() <= 0) {
        emit lastNotificationID(-1);
    } else {
        QListWidgetItem* topItem = listWidget->item(0);
        emit lastNotificationID(topItem->data(IR_ID).toInt());
    }
}


/**
 * @brief NotificationDialog::clearSelected
 */
void NotificationDialog::clearSelected()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    QList<NOTIFICATION_SEVERITY> removedSeverities;

    // delete selected items
    while (!selectedItems.isEmpty()) {
        QListWidgetItem* item = selectedItems.takeFirst();
        NOTIFICATION_SEVERITY severity = (NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
        if (severity != NS_ERROR) {
            if (!removedSeverities.contains(severity)) {
                removedSeverities.append(severity);
            }
            removeItem(item);
        }
    }

    updateSeverityActions(removedSeverities);
}


/**
 * @brief NotificationDialog::clearVisible
 */
void NotificationDialog::clearVisible()
{
    QList<QListWidgetItem*> visibleItems;
    QList<NOTIFICATION_SEVERITY> removedSeverities;

    for (int i = 0; i < listWidget->count(); i++) {
        if (!listWidget->isRowHidden(i)) {
            QListWidgetItem* item = listWidget->item(i);
            NOTIFICATION_SEVERITY severity = (NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
            if (severity != NS_ERROR) {
                visibleItems.append(item);
                if (!removedSeverities.contains(severity)) {
                    removedSeverities.append(severity);
                }
            }
        }
    }

    // delete visible items
    while (!visibleItems.isEmpty()) {
        removeItem(visibleItems.takeFirst());
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
    foreach (NotificationItem* item, itemsToDelete) {
        // TODO - delete item here
    }
}


/**
 * @brief NotificationDialog::notificationItemAdded
 * @param item
 */
void NotificationDialog::notificationItemAdded(NotificationObject* item)
{
    if (item) {
        constructNotificationItem(item->ID(), item->severity(), item->title(), item->description(), item->iconPath(), item->iconName(), item->entityID());
    }
}


/**
 * @brief NotificationDialog::clearNotificationsOfSeverity
 * @param severity
 */
void NotificationDialog::clearNotificationsOfSeverity(NOTIFICATION_SEVERITY severity)
{
    QList<QListWidgetItem*> itemsOfSeverity;
    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        NOTIFICATION_SEVERITY itemSeverity = (NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
        if (itemSeverity == severity) {
            itemsOfSeverity.append(item);
        }
    }

    // delete items of the provided severity
    while (!itemsOfSeverity.isEmpty()) {
        removeItem(itemsOfSeverity.takeFirst());
    }

    updateSeverityAction(severity);
}


/**
 * @brief NotificationDialog::clearAll
 */
void NotificationDialog::clearAll()
{
    notificationHash.clear();
    notificationIDHash.clear();
    listWidget->clear();
    checkedActions.clear();
    updateSeverityActions(NotificationManager::getNotificationSeverities());
    updateVisibilityCount(0, true);
    // TODO - Need to clear notification items here!!!
    // and update filter lists for checked states
}


/**
 * @brief NotificationDialog::removeNotificationItem
 * Remove the notification item with the provided ID from the list widget and the hash.
 * @param ID
 */
void NotificationDialog::removeNotificationItem(int ID)
{
    if (notificationIDHash.contains(ID)) {
        QListWidgetItem* item = notificationIDHash.take(ID);
        removeItem(item);
    }
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
 * Remove item from the list widget and the hash and then delete it.
 * @param item
 */
void NotificationDialog::removeItem(QListWidgetItem* item)
{
    if (listWidget && item) {

        // remove from notifications hash
        NOTIFICATION_SEVERITY severity = (NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
        notificationHash.remove(severity, item);

        // remove from IDs hash
        int ID = item->data(IR_ID).toInt();
        if (notificationIDHash.contains(ID)) {
            notificationIDHash.remove(ID);
        }

        int row = listWidget->row(item);
        if (!listWidget->isRowHidden(row)) {
            updateVisibilityCount(-1);
        }

        // remove from list widget then delete item
        delete listWidget->takeItem(row);
        emit itemDeleted(ID);
    }
}


/**
 * @brief NotificationDialog::getSeverityAction
 * @param severity
 * @return
 */
QAction* NotificationDialog::getSeverityAction(NOTIFICATION_SEVERITY severity) const
{
    return severityActionHash.value(severity, 0);
}


/**
 * @brief NotificationDialog::getActionIcon
 * @param severity
 * @return
 */
QPair<QString, QString> NotificationDialog::getActionIcon(NOTIFICATION_SEVERITY severity) const
{
    QPair<QString, QString> iconPath;
    iconPath.first = "Actions";

    switch (severity) {
    case NS_INFO:
        iconPath.second = "Information";
        break;
    case NS_WARNING:
        iconPath.second = "Warning";
        break;
    case NS_ERROR:
        iconPath.second = "Error";
        break;
    default:
        iconPath.second = "Help";
        break;
    }

    return iconPath;
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
 * @brief NotificationDialog::listItemClicked
 * @param item
 */
void NotificationDialog::notificationItemClicked(QListWidgetItem* item)
{
    /*
    if (item) {
        int entityID = item->data(IR_ENTITYID).toInt();
        if (entityID > 0) {
            emit centerOn(entityID);
        }
    }
    */
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
    QAction* severityAction = getSeverityAction(severity);
    if (severityAction) {
        int count = notificationHash.values(severity).length();
        QString text = "(" % QString::number(count) % ")";
        severityAction->setText(text);
        emit updateSeverityCount(severity, count);
    }
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

    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    listWidget->setUniformItemSizes(true);
    listWidget->setFocusPolicy(Qt::NoFocus);

    topToolbar = new QToolBar(this);
    topToolbar->setIconSize(QSize(20,20));

    centerOnAction = topToolbar->addAction("");
    centerOnAction->setToolTip("Center View Aspect On Selected Item");
    centerOnAction->setEnabled(false);

    popupAction = topToolbar->addAction("");
    popupAction->setToolTip("View Selected Item In New Window");
    popupAction->setEnabled(false);

    bottomToolbar = new QToolBar(this);
    bottomToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    bottomToolbar->setIconSize(QSize(20,20));

    foreach(NOTIFICATION_SEVERITY severity, NotificationManager::getNotificationSeverities()){
        QAction* action = severityActionHash.value(severity, 0);
        if (action) {
            bottomToolbar->addAction(action);
            action->setToolTip("Show/Hide " + NotificationManager::getSeverityString(severity) + " Notifications");
        }
    }

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
    bottomLayout->addWidget(bottomToolbar, 1);
    bottomLayout->addWidget(iconOnlyToolbar);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(listWidget, 1);
    mainLayout->addLayout(bottomLayout);

    connect(listWidget, &QListWidget::itemSelectionChanged, this, &NotificationDialog::listSelectionChanged);
    //connect(listWidget, &QListWidget::itemClicked, this, &NotificationDialog::notificationItemClicked);
    connect(centerOnAction, &QAction::triggered, this, &NotificationDialog::viewSelection);
    connect(popupAction, &QAction::triggered, this, &NotificationDialog::viewSelection);
    connect(clearSelectedAction, &QAction::triggered, this, &NotificationDialog::clearSelected);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);

    setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);
}


/**
 * @brief NotificationDialog::setupLayout2
 */
void NotificationDialog::setupLayout2()
{
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

    topButtonsToolbar = new QToolBar(this);
    filtersButton = new QToolButton(this);
    filtersButton->setPopupMode(QToolButton::InstantPopup);
    filtersButton->setFont(QFont(font().family(), 10));
    filtersButton->setText("Filters");
    filtersButton->setMenu(filtersMenu);
    topButtonsToolbar->addWidget(filtersButton);

    filtersToolbar = new QToolBar(this);
    filtersToolbar->setOrientation(Qt::Vertical);
    filtersToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filtersToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    filtersToolbar->setIconSize(QSize(20,20));

    QScrollArea* filtersArea = new QScrollArea(this);
    filtersArea->setWidget(filtersToolbar);
    filtersArea->setWidgetResizable(true);

    QToolButton* allButton = new QToolButton(this);
    allButton->setText("All");
    allButton->setProperty("iconPath", "Actions");
    allButton->setProperty("iconName", "Menu");
    allAction = constructFilterButtonAction(allButton);
    setActionButtonChecked(allAction, true);

    // setup the SEVERITY, TYPE, and CATEGORY filter actions/buttons in that order
    NotificationManager* manager = NotificationManager::manager();
    foreach (NOTIFICATION_SEVERITY severity, manager->getNotificationSeverities()) {
        QString iconName = manager->getSeverityString(severity);
        constructFilterButton(IR_SEVERITY, severity, manager->getSeverityString(severity), "", iconName);
        severityCheckedStates[severity] = false;
    }
    foreach (NOTIFICATION_TYPE2 type, manager->getNotificationTypes()) {
        QString iconName;
        if (type == NT_MODEL) {
            iconName = "Model";
        } else if (type == NT_APPLICATION) {
            iconName = "Rename";
        }
        constructFilterButton(IR_TYPE, type, manager->getTypeString(type), "", iconName);
        typeCheckedStates[type] = false;
    }
    foreach (NOTIFICATION_CATEGORY category, manager->getNotificationCategories()) {
        constructFilterButton(IR_CATEGORY, category, manager->getCategoryString(category));
        categoryCheckedStates[category] = false;
    }

    QFrame* displayWidget = new QFrame(this);
    displayWidget->setStyleSheet("QFrame{ background: rgba(0,0,0,0); }");
    itemsLayout = new QVBoxLayout(displayWidget);
    itemsLayout->setMargin(0);
    itemsLayout->setSpacing(0);
    itemsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    itemsLayout->setSizeConstraint(QLayout::SetMinimumSize);

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
}


/**
 * @brief NotificationDialog::constructFilterButton
 * @param role
 * @param roleVal
 * @param label
 * @param iconPath
 * @param iconName
 */
void NotificationDialog::constructFilterButton(NotificationDialog::ITEM_ROLES role, int roleVal, QString label, QString iconPath, QString iconName)
{
    int index = indexMap.value(role, -1);
    QActionGroup* group = actionGroups.value(index, 0);

    // action group and separator combo doesn't exist yet
    if (!group) {
        group = new QActionGroup(this);
        group->setExclusive(false);
        QAction* separator = filtersToolbar->addSeparator();
        index = indexMap.size();
        indexMap[role] = index;
        actionGroups.insert(index, group);
        groupSeparators.insert(index, separator);
        prevGroupIndex[separator] = index - 1;
    }

    // set default icon
    if (iconPath.isEmpty()) {
        iconPath = "Actions";
    }
    if (iconName.isEmpty()) {
        iconName = "Help";
    }

    QToolButton* button = new QToolButton(this);
    button->setText(label);
    button->setProperty("iconPath", iconPath);
    button->setProperty("iconName", iconName);

    QAction* action = constructFilterButtonAction(button);
    group->addAction(action);
    action->setProperty(ROLE, role);
    action->setProperty(ROLE_VAL, roleVal);
}


/**
 * @brief NotificationDialog::constructFilterButtonAction
 * This constructs and returns an action that is linked to the provided tool button.
 * It sets up the button and adds it to the hash and the filters toolbar.
 * @param button
 */
QAction* NotificationDialog::constructFilterButtonAction(QToolButton* button)
{
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    button->setCheckable(true);
    button->setChecked(false);

    QAction* action = filtersToolbar->addWidget(button);
    action->setCheckable(true);
    action->setChecked(false);

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
        //qDebug() << "action.isChecked: " << action->isChecked();
        //qDebug() << "button.isChecked: " << button->isChecked();
        //button->updateGeometry();
        //button->update();
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


/**
 * @brief NotificationDialog::constructNotificationItem
 * @param ID
 * @param severity
 * @param title
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 */
void NotificationDialog::constructNotificationItem(int ID, NOTIFICATION_SEVERITY severity, QString title, QString description, QString iconPath, QString iconName, int entityID)
{
    if (iconPath.isEmpty() || iconName.isEmpty()) {
        iconPath = getActionIcon(severity).first;
        iconName = getActionIcon(severity).second;
    }

    QListWidgetItem* listItem = new QListWidgetItem();
    int elapsedTime = NotificationManager::projectTime()->msecsTo((new QTime)->currentTime());

    // set item data
    listItem->setData(IR_ID, ID);
    listItem->setData(IR_SEVERITY, severity);
    listItem->setData(IR_ICONPATH, iconPath);
    listItem->setData(IR_ICONNAME, iconName);
    listItem->setData(IR_ENTITYID, entityID);
    listItem->setData(IR_TIMESTAMP, elapsedTime);

    QLabel* textLabel = new QLabel(" [" % title % "] " % description, this);
    QLabel* timeLabel = new QLabel(QString::number(elapsedTime), this);

    QFrame* itemFrame = new QFrame(this);
    QHBoxLayout* frameLayout = new QHBoxLayout(itemFrame);
    frameLayout->setMargin(0);
    //frameLayout->addSpacerItem(new QSpacerItem(5,0));
    frameLayout->addWidget(textLabel, 1);
    frameLayout->addWidget(timeLabel);

    listItem->setIcon(Theme::theme()->getIcon(iconPath, iconName));
    listItem->setSizeHint(QSize(itemFrame->sizeHint().width(), 35));
    listWidget->insertItem(0, listItem);
    listWidget->setItemWidget(listItem, itemFrame);

    // store in map and hash
    notificationHash.insertMulti(severity, listItem);
    notificationIDHash[ID] = listItem;

    QAction* severityAction = getSeverityAction(severity);
    if (severityAction) {
        bool actionVisible = severityAction->isChecked();
        listWidget->setRowHidden(0, !actionVisible);
        updateSeverityAction(severity);
        if (actionVisible) {
            updateVisibilityCount(1);
        }
    }

    NotificationItem* item = new NotificationItem(ID, description, iconPath, iconName, entityID, severity, NT_MODEL, NC_NOCATEGORY, this);
    itemsLayout->addWidget(item);

    notificationItems[ID] = item;

    connect(this, &NotificationDialog::filtersCleared, item, &NotificationItem::show);
    connect(this, &NotificationDialog::severityFiltersChanged, item, &NotificationItem::severityFilterToggled);
    connect(this, &NotificationDialog::typeFiltersChanged, item, &NotificationItem::typeFilterToggled);
    connect(this, &NotificationDialog::categoryFiltersChanged, item, &NotificationItem::categoryFilterToggled);
}
