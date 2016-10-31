#include "notificationdialog.h"
#include "../../theme.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QTime>
#include <QStringBuilder>
#include <QApplication>

#define ICON_SIZE 24

/**
 * @brief NotificationDialog::NotificationDialog
 * @param parent
 */
NotificationDialog::NotificationDialog(QWidget *parent) :
    QDialog(parent)
{
    severityActionMapper = new QSignalMapper(this);

    foreach (NotificationManager::NOTIFICATION_SEVERITY severity, NotificationManager::getNotificationSeverities()) {
        QAction* action = new QAction(this);
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, &QAction::toggled, severityActionMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        severityActionMapper->setMapping(action, severity);
        severityActionHash.insert(severity, action);
        updateSeverityAction(severity);
    }

    setupLayout();
    setWindowTitle("Notifications");

    connect(severityActionMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped),this, &NotificationDialog::severityActionToggled);
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    connect(NotificationManager::manager(), &NotificationManager::notificationItemAdded, this, &NotificationDialog::notificationItemAdded);
    connect(NotificationManager::manager(), &NotificationManager::req_lastNotificationID, this, &NotificationDialog::getLastNotificationID);
    connect(this, &NotificationDialog::lastNotificationID, NotificationManager::manager(), &NotificationManager::setLastNotificationItem);
    connect(this, &NotificationDialog::itemDeleted, NotificationManager::manager(), &NotificationManager::deleteNotification);

    themeChanged();
    updateVisibilityCount(0, true);

    // construct items for notifications that were received before this dialog was constructed
    foreach (NotificationItem* item, NotificationManager::getNotificationItems()) {
        notificationItemAdded(item);
    }
}


/**
 * @brief NotificationDialog::severityActionToggled
 * @param actionSeverity
 */
void NotificationDialog::severityActionToggled(int actionSeverity)
{
    NotificationManager::NOTIFICATION_SEVERITY severity = (NotificationManager::NOTIFICATION_SEVERITY) actionSeverity;
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
 * @brief NotificationDialog::themeChanged
 */
void NotificationDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QListWidget{ border: 1px solid " + theme->getDisabledBackgroundColorHex() + "; }"
                  + theme->getAltAbstractItemViewStyleSheet()
                  + theme->getDialogStyleSheet()
                  + "QLabel{ background: rgba(0,0,0,0); color:" + theme->getTextColorHex()+ ";}");

    /*
    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                              "QToolButton{ padding: 2px; border-radius:" + theme->getSharpCornerRadius() + "; color:" + theme->getTextColorHex() + ";}"
                              "QToolButton::checked{ background:" + theme->getPressedColorHex() + ";color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}"
                              "QToolButton:hover{ background:" + theme->getHighlightColorHex() + "; color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}");
                              */

    bottomToolbar->setStyleSheet(theme->getToolBarStyleSheet());
    iconOnlyToolbar->setStyleSheet(theme->getToolBarStyleSheet());

    clearSelectedAction->setIcon(theme->getIcon("Actions", "Delete"));
    clearVisibleAction->setIcon(theme->getIcon("Actions", "Clear"));

    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        if (item) {
            QString iconPath = item->data(IR_ICONPATH).toString();
            QString iconName = item->data(IR_ICONNAME).toString();
            item->setIcon(theme->getIcon(iconPath, iconName));
        }
    }

    foreach (NotificationManager::NOTIFICATION_SEVERITY severity, NotificationManager::getNotificationSeverities()) {
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
    if (csEnabled != (!listWidget->selectedItems().isEmpty())) {
        clearSelectedAction->setEnabled(!csEnabled);
    }
}


/**
 * @brief NotificationDialog::toggleVisibility
 */
void NotificationDialog::toggleVisibility()
{
    setVisible(!isVisible());
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
    QList<NotificationManager::NOTIFICATION_SEVERITY> removedSeverities;

    // delete selected items
    while (!selectedItems.isEmpty()) {
        QListWidgetItem* item = selectedItems.takeFirst();
        NotificationManager::NOTIFICATION_SEVERITY severity = (NotificationManager::NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
        if (severity != NT_ERROR) {
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
    QList<NotificationManager::NOTIFICATION_SEVERITY> removedSeverities;

    for (int i = 0; i < listWidget->count(); i++) {
        if (!listWidget->isRowHidden(i)) {
            QListWidgetItem* item = listWidget->item(i);
            NotificationManager::NOTIFICATION_SEVERITY severity = (NotificationManager::NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
            if (severity != NT_ERROR) {
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
 * @brief NotificationDialog::notificationItemAdded
 * @param item
 */
void NotificationDialog::notificationItemAdded(NotificationItem* item)
{
    if (item) {
        constructNotificationItem(item->ID(), item->severity(), item->title(), item->description(), item->iconPath(), item->iconName(), item->entityID());
    }
}


/**
 * @brief NotificationDialog::clearNotificationsOfSeverity
 * @param severity
 */
void NotificationDialog::clearNotificationsOfSeverity(NotificationManager::NOTIFICATION_SEVERITY severity)
{
    QList<QListWidgetItem*> itemsOfSeverity;
    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        NotificationManager::NOTIFICATION_SEVERITY itemSeverity = (NotificationManager::NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
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
    updateSeverityActions(NotificationManager::getNotificationSeverities());
    updateVisibilityCount(0, true);
}


/**
 * @brief NotificationDialog::addNotificationItem
 * @param ID
 * @param severity
 * @param title
 * @param description
 * @param iconPath
 * @param entityID
 */
void NotificationDialog::addNotificationItem(int ID, NotificationManager::NOTIFICATION_SEVERITY severity, QString title, QString description, QPair<QString, QString> iconPath, int entityID)
{
    if (iconPath.first.isEmpty() || iconPath.second.isEmpty()) {
        iconPath = getActionIcon(severity);
    }

    QListWidgetItem* listItem = new QListWidgetItem();

    //Set Data to item.
    listItem->setData(IR_SEVERITY, severity);
    listItem->setData(IR_ICONPATH, iconPath.first);
    listItem->setData(IR_ICONNAME, iconPath.second);
    listItem->setData(IR_ENTITYID, entityID);
    listItem->setData(IR_ID, ID);

    /*
    QLabel* textLabel = new QLabel("[" % title % "] " % description, this);
    QLabel* timeLabel = new QLabel((new QTime())->currentTime().toString(), this);

    QFrame* itemFrame = new QFrame(this);
    QHBoxLayout* frameLayout = new QHBoxLayout(itemFrame);
    frameLayout->setMargin(0);
    frameLayout->addWidget(textLabel, 1);
    frameLayout->addWidget(timeLabel);

    listItem->setSizeHint(itemFrame->sizeHint());
    */

    //Set the Icon and Text
    listItem->setText("[" % title % "] " % description);
    listItem->setIcon(Theme::theme()->getIcon(iconPath));

    //Insert in the listWidget at 0
    listWidget->insertItem(0, listItem);
    //listWidget->setItemWidget(listItem, itemFrame);

    //Put in the multimap and hash
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
 * @brief NotificationDialog::removeItem
 * Remove item from the list widget and the hash and then delete it.
 * @param item
 */
void NotificationDialog::removeItem(QListWidgetItem* item)
{
    if (listWidget && item) {

        // remove from notifications hash
        NotificationManager::NOTIFICATION_SEVERITY severity = (NotificationManager::NOTIFICATION_SEVERITY) item->data(IR_SEVERITY).toInt();
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
QAction* NotificationDialog::getSeverityAction(NotificationManager::NOTIFICATION_SEVERITY severity) const
{
    return severityActionHash.value(severity, 0);
}


/**
 * @brief NotificationDialog::getActionIcon
 * @param severity
 * @return
 */
QPair<QString, QString> NotificationDialog::getActionIcon(NotificationManager::NOTIFICATION_SEVERITY severity) const
{
    QPair<QString, QString> iconPath;
    iconPath.first = "Actions";

    switch (severity) {
    case NotificationManager::NS_INFO:
        iconPath.second = "Info";
        break;
    case NotificationManager::NS_WARNING:
        iconPath.second = "Warning";
        break;
    case NotificationManager::NS_ERROR:
        iconPath.second = "Failure";
        break;
    default:
        iconPath.second = "Help";
        break;
    }

    return iconPath;
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
    if (item) {
        int entityID = item->data(IR_ENTITYID).toInt();
        if (entityID > 0) {
            emit centerOn(entityID);
        }
    }
}


/**
 * @brief NotificationDialog::updateSeverityActions
 * @param severities
 */
void NotificationDialog::updateSeverityActions(QList<NotificationManager::NOTIFICATION_SEVERITY> severities)
{
    foreach (NotificationManager::NOTIFICATION_SEVERITY severity, severities) {
        updateSeverityAction(severity);
    }
}


/**
 * @brief NotificationDialog::updateSeverityAction
 * This updates the text (displaying the items count) in the button for the particular severity.
 * @param severity
 */
void NotificationDialog::updateSeverityAction(NotificationManager::NOTIFICATION_SEVERITY severity)
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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(DIALOG_SPACING);

    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    listWidget->setUniformItemSizes(true);
    listWidget->setFocusPolicy(Qt::NoFocus);

    bottomToolbar = new QToolBar(this);
    bottomToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    bottomToolbar->setIconSize(QSize(20,20));

    foreach(NotificationManager::NOTIFICATION_SEVERITY severity, NotificationManager::getNotificationSeverities()){
        QAction* action = severityActionHash.value(severity, 0);
        if (action) {
            bottomToolbar->addAction(action);
            action->setToolTip("Show/Hide " + NotificationManager::getNotificationSeverityString(severity) + " Notifications");
        }
    }

    iconOnlyToolbar = new QToolBar(this);
    iconOnlyToolbar->setIconSize(QSize(20,20));
    clearSelectedAction = iconOnlyToolbar->addAction("Clear Selected");
    clearVisibleAction = iconOnlyToolbar->addAction("Clear Visible");

    clearSelectedAction->setToolTip("Clear Selected Items");
    clearVisibleAction->setToolTip("Clear Visible Items");

    clearSelectedAction->setEnabled(false);
    clearVisibleAction->setEnabled(false);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(bottomToolbar, 1);
    bottomLayout->addWidget(iconOnlyToolbar);

    mainLayout->addWidget(listWidget, 1);
    mainLayout->addLayout(bottomLayout);

    connect(listWidget, &QListWidget::itemSelectionChanged, this, &NotificationDialog::listSelectionChanged);
    connect(listWidget, &QListWidget::itemClicked, this, &NotificationDialog::notificationItemClicked);
    connect(clearSelectedAction, &QAction::triggered, this, &NotificationDialog::clearSelected);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);

    setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);
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
void NotificationDialog::constructNotificationItem(int ID, NotificationManager::NOTIFICATION_SEVERITY severity, QString title, QString description, QString iconPath, QString iconName, int entityID)
{
    if (iconPath.isEmpty() || iconName.isEmpty()) {
        iconPath = getActionIcon(severity).first;
        iconName = getActionIcon(severity).second;
    }

    QListWidgetItem* listItem = new QListWidgetItem();

    // set item data
    listItem->setData(IR_ID, ID);
    listItem->setData(IR_SEVERITY, severity);
    listItem->setData(IR_ICONPATH, iconPath);
    listItem->setData(IR_ICONNAME, iconName);
    listItem->setData(IR_ENTITYID, entityID);

    QLabel* textLabel = new QLabel("[" % title % "] " % description, this);
    QLabel* timeLabel = new QLabel((new QTime())->currentTime().toString(), this);

    QFrame* itemFrame = new QFrame(this);
    QHBoxLayout* frameLayout = new QHBoxLayout(itemFrame);
    frameLayout->setMargin(0);
    //frameLayout->addSpacerItem(new QSpacerItem(2,0));
    frameLayout->addWidget(textLabel, 1);
    frameLayout->addWidget(timeLabel);

    listItem->setIcon(Theme::theme()->getIcon(iconPath, iconName));
    listItem->setSizeHint(QSize(itemFrame->sizeHint().width(), 35));
    listWidget->insertItem(0, listItem);
    listWidget->setItemWidget(listItem, itemFrame);

    //Put in the multimap and hash
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
}
