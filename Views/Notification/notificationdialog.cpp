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
NotificationDialog::NotificationDialog(QWidget *parent) : QDialog(parent)
{
    typeActionMapper = new QSignalMapper(this);

    foreach (NOTIFICATION_TYPE type, GET_NOTIFICATION_TYPES()) {
        QAction* action = new QAction(this);
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, &QAction::toggled, typeActionMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        typeActionMapper->setMapping(action, type);
        typeActionHash.insert(type, action);
        updateTypeAction(type);
    }

    setupLayout();
    setWindowTitle("Notifications");

    connect(typeActionMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped),this, &NotificationDialog::typeActionToggled);
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();
    updateVisibilityCount(0, true);
}


/**
 * @brief NotificationDialog::typeActionToggled
 * @param t
 */
void NotificationDialog::typeActionToggled(int t)
{
    NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) t;
    QAction* action = typeActionHash.value(type, 0);
    bool clearSelection = false;
    if (action) {
        bool visible = action->isChecked();
        foreach (QListWidgetItem* item, notificationHash.values(type)) {
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

    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                              "QToolButton{ padding: 2px; border-radius:" + theme->getSharpCornerRadius() + "; color:" + theme->getTextColorHex() + ";}"
                              "QToolButton::checked{ background:" + theme->getPressedColorHex() + ";color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}"
                              "QToolButton:hover{ background:" + theme->getHighlightColorHex() + "; color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}");

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

    foreach (NOTIFICATION_TYPE type, GET_NOTIFICATION_TYPES()) {
        QAction* action = typeActionHash.value(type, 0);
        if (action) {
             QPair<QString, QString> iconPath = getActionIcon(type);
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
 * @brief NotificationDialog::clearSelected
 */
void NotificationDialog::clearSelected()
{
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    QList<NOTIFICATION_TYPE> removedTypes;

    // delete selected items
    while (!selectedItems.isEmpty()) {
        QListWidgetItem* item = selectedItems.takeFirst();
        NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();
        if (type != NT_ERROR) {
            if (!removedTypes.contains(type)) {
                removedTypes.append(type);
            }
            removeItem(item);
        }
    }

    updateTypeActions(removedTypes);
}


/**
 * @brief NotificationDialog::clearVisible
 */
void NotificationDialog::clearVisible()
{
    QList<QListWidgetItem*> visibleItems;
    QList<NOTIFICATION_TYPE> removedTypes;

    for (int i = 0; i < listWidget->count(); i++) {
        if (!listWidget->isRowHidden(i)) {
            QListWidgetItem* item = listWidget->item(i);
            NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();
            if (type != NT_ERROR) {
                visibleItems.append(item);
                if (!removedTypes.contains(type)) {
                    removedTypes.append(type);
                }
            }
        }
    }

    // delete visible items
    while (!visibleItems.isEmpty()) {
        removeItem(visibleItems.takeFirst());
    }

    updateTypeActions(removedTypes);
}


/**
 * @brief NotificationDialog::clearNotifications
 */
void NotificationDialog::clearNotifications()
{
    QObject* senderObj = sender();
    if (senderObj == clearInformations) {
        clearNotificationsOfType(NT_INFO);
    } else if (senderObj == clearWarnings){
        clearNotificationsOfType(NT_WARNING);
    }
}


/**
 * @brief NotificationDialog::clearNotificationsOfType
 * @param type
 */
void NotificationDialog::clearNotificationsOfType(NOTIFICATION_TYPE type)
{
    QList<QListWidgetItem*> itemsOfType;
    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        NOTIFICATION_TYPE itemType = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();
        if (itemType == type) {
            itemsOfType.append(item);
        }
    }

    // delete items of the provided type
    while (!itemsOfType.isEmpty()) {
        removeItem(itemsOfType.takeFirst());
    }

    updateTypeAction(type);
}


/**
 * @brief NotificationDialog::clearAll
 */
void NotificationDialog::clearAll()
{
    notificationHash.clear();
    notificationIDHash.clear();
    listWidget->clear();
    updateTypeActions(GET_NOTIFICATION_TYPES());
    updateVisibilityCount(0, true);
}


/**
 * @brief NotificationDialog::addNotificationItem
 * @param ID
 * @param type
 * @param title
 * @param description
 * @param iconPath
 * @param entityID
 */
void NotificationDialog::addNotificationItem(int ID, NOTIFICATION_TYPE type, QString title, QString description, QPair<QString, QString> iconPath, int entityID)
{
    if (iconPath.first.isEmpty() || iconPath.second.isEmpty()) {
        iconPath = getActionIcon(type);
    }

    QListWidgetItem* listItem = new QListWidgetItem();

    //Set Data to item.
    listItem->setData(IR_TYPE, type);
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
    notificationHash.insertMulti(type, listItem);
    notificationIDHash[ID] = listItem;

    QAction* typeAction = getTypeAction(type);
    if (typeAction) {
        bool actionVisible = typeAction->isChecked();
        listWidget->setRowHidden(0, !actionVisible);
        updateTypeAction(type);
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
 * @brief NotificationDialog::getTopNotificationID
 * @return
 */
int NotificationDialog::getTopNotificationID()
{
    if (listWidget->count() <= 0) {
        return -1;
    }
    QListWidgetItem* topItem = listWidget->item(0);
    return topItem->data(IR_ID).toInt();
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
        NOTIFICATION_TYPE type = (NOTIFICATION_TYPE) item->data(IR_TYPE).toInt();
        notificationHash.remove(type, item);

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
 * @brief NotificationDialog::getTypeAction
 * @param type
 * @return
 */
QAction* NotificationDialog::getTypeAction(NOTIFICATION_TYPE type) const
{
    return typeActionHash.value(type, 0);
}


/**
 * @brief NotificationDialog::getActionIcon
 * @param type
 * @return
 */
QPair<QString, QString> NotificationDialog::getActionIcon(NOTIFICATION_TYPE type) const
{
    QPair<QString, QString> iconPath;
    iconPath.first = "Actions";

    switch (type) {
    case NT_INFO:
        iconPath.second = "Info";
        break;
    case NT_WARNING:
        iconPath.second = "Warning";
        break;
    case NT_CRITICAL:
        iconPath.second = "Critical";
        break;
    case NT_ERROR:
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
 * @brief NotificationDialog::updateTypeActions
 * @param types
 */
void NotificationDialog::updateTypeActions(QList<NOTIFICATION_TYPE> types)
{
    foreach (NOTIFICATION_TYPE type, types) {
        updateTypeAction(type);
    }
}


/**
 * @brief NotificationDialog::updateTypeAction
 * @param type
 */
void NotificationDialog::updateTypeAction(NOTIFICATION_TYPE type)
{
    QAction* typeAction = getTypeAction(type);
    if (typeAction) {
        int count = notificationHash.values(type).length();
        QString text = "(" % QString::number(count) % ")";
        typeAction->setText(text);
        emit updateTypeCount(type, count);
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

    topToolbar = new QToolBar(this);
    topToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    topToolbar->hide();

    //QWidget* w = new QWidget(this);
    //w->setStyleSheet("background: rgba(0,0,0,0);");
    //w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    bottomToolbar = new QToolBar(this);
    //bottomToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    bottomToolbar->setIconSize(QSize(20,20));

    foreach(NOTIFICATION_TYPE type, GET_NOTIFICATION_TYPES()){
        QAction* action = typeActionHash.value(type, 0);
        if (action) {
            //topToolbar->addAction(action);
            bottomToolbar->addAction(action);
            action->setToolTip("Show/Hide " + GET_NOTIFICATION_TYPE_STRING(type) + " Notifications");
        }
    }

    //bottomToolbar->addWidget(w);

    iconOnlyToolbar = new QToolBar(this);
    iconOnlyToolbar->setIconSize(QSize(20,20));
    clearSelectedAction = iconOnlyToolbar->addAction("Clear Selected");
    clearVisibleAction = iconOnlyToolbar->addAction("Clear Visible");

    //bottomToolbar->addAction(clearSelectedAction);
    //bottomToolbar->addAction(clearVisibleAction);

    //clearSelectedAction = bottomToolbar->addAction("Clear Selected");
    //clearVisibleAction = bottomToolbar->addAction("Clear Visible");
    clearInformations = bottomToolbar->addAction("Clear Informations");
    clearWarnings = bottomToolbar->addAction("Clear Warnings");

    clearSelectedAction->setToolTip("Clear Selected Items");
    clearVisibleAction->setToolTip("Clear Visible Items");
    clearInformations->setToolTip("Clear Information Items");
    clearWarnings->setToolTip("Clear Warning Items");

    clearSelectedAction->setEnabled(false);
    clearVisibleAction->setEnabled(false);

    //clearVisibleAction->setVisible(false);
    clearInformations->setVisible(false);
    clearWarnings->setVisible(false);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(bottomToolbar, 1);
    bottomLayout->addWidget(iconOnlyToolbar);

    mainLayout->addWidget(topToolbar, 0, Qt::AlignHCenter);
    mainLayout->addWidget(listWidget, 1);
    mainLayout->addLayout(bottomLayout);
    //mainLayout->addWidget(bottomToolbar);
    //mainLayout->addWidget(bottomToolbar, 0, Qt::AlignRight);

    connect(listWidget, &QListWidget::itemSelectionChanged, this, &NotificationDialog::listSelectionChanged);
    connect(listWidget, &QListWidget::itemClicked, this, &NotificationDialog::notificationItemClicked);
    connect(clearSelectedAction, &QAction::triggered, this, &NotificationDialog::clearSelected);
    connect(clearVisibleAction, &QAction::triggered, this, &NotificationDialog::clearVisible);
    connect(clearInformations, &QAction::triggered, this, &NotificationDialog::clearNotifications);
    connect(clearWarnings, &QAction::triggered, this, &NotificationDialog::clearNotifications);

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
