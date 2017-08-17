#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../ViewController/viewcontroller.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationobject.h"
#include "../../Views/Notification/notificationdialog.h"
#include "../../Views/Notification/notificationtoolbar.h"
#include "../../Views/Notification/notificationpopup.h"
#include "../../Widgets/Dialogs/popupwidget.h"

NotificationManager* NotificationManager::managerSingleton = 0;
NotificationObject* NotificationManager::lastNotificationObject = 0;
QHash<int, NotificationObject*> NotificationManager::notificationObjects;


/**
 * @brief NotificationManager::NotificationManager
 * @param controller
 */
NotificationManager::NotificationManager(ViewController* controller)
{
    viewController = controller;
    getNotificationPopup();
}


/**
 * @brief NotificationManager::~NotificationManager
 */
NotificationManager::~NotificationManager() {}


/**
 * @brief NotificationManager::construct_singleton
 * @param controller
 * @return
 */
bool NotificationManager::construct_singleton(ViewController* controller)
{
    if (!managerSingleton) {
        managerSingleton = new NotificationManager(controller);
        return true;
    }
    return false;
}


/**
 * @brief NotificationManager::destruct_singleton
 */
void NotificationManager::destruct_singleton()
{
    if (managerSingleton) {
        delete managerSingleton;
    }
    managerSingleton = 0;
}


/**
 * @brief NotificationManager::manager
 * @return
 */
NotificationManager* NotificationManager::manager()
{
    return managerSingleton;
}


/**
 * @brief NotificationManager::constructPanel
 * @return
 */
NotificationDialog* NotificationManager::constructPanel()
{
    NotificationDialog* panel = new NotificationDialog();
    connect(manager(), &NotificationManager::req_lastNotificationID, panel, &NotificationDialog::getLastNotificationID);
    connect(manager(), &NotificationManager::notificationItemAdded, panel, &NotificationDialog::notificationAdded);
    connect(manager(), &NotificationManager::notificationDeleted, panel, &NotificationDialog::notificationDeleted);
    connect(manager(), &NotificationManager::selectionChanged, panel, &NotificationDialog::entitySelectionChanged);
    connect(panel, &NotificationDialog::deleteNotification, manager(), &NotificationManager::deleteNotification);
    connect(panel, &NotificationDialog::lastNotificationID, manager(), &NotificationManager::setLastNotificationItem);
    connect(panel, &NotificationDialog::mouseEntered, manager(), &NotificationManager::notificationSeen);
    return panel;
}


/**
 * @brief NotificationManager::constructToolbar
 * @return
 */
NotificationToolbar* NotificationManager::constructToolbar()
{
    NotificationToolbar* toolbar = new NotificationToolbar();
    connect(manager(), &NotificationManager::notificationAlert, toolbar, &NotificationToolbar::notificationReceived);
    connect(manager(), &NotificationManager::notificationSeen, toolbar, &NotificationToolbar::notificationsSeen);
    connect(manager(), &NotificationManager::updateSeverityCount, toolbar, &NotificationToolbar::updateSeverityCount);
    connect(manager(), &NotificationManager::lastNotificationDeleted, toolbar, &NotificationToolbar::lastNotificationDeleted);
    connect(toolbar, &NotificationToolbar::showLastNotification, manager(), &NotificationManager::popupLatestNotification);
    return toolbar;
}


/**
 * @brief NotificationManager::getNotificationItems
 * @return
 */
QList<NotificationObject*> NotificationManager::getNotificationItems()
{
    return notificationObjects.values();
}


/**
 * @brief NotificationManager::getNotificationsOfType
 * @param type
 * @return
 */
QList<int> NotificationManager::getNotificationsOfType(NOTIFICATION_TYPE type)
{
    QList<int> IDs;
    foreach (NotificationObject* obj, notificationObjects.values()) {
        if (obj->type() == type) {
            IDs.append(obj->ID());
        }
    }
    return IDs;
}


/**
 * @brief NotificationManager::getNotificationsOfSeverity
 * @param severity
 * @return
 */
QList<int> NotificationManager::getNotificationsOfSeverity(NOTIFICATION_SEVERITY severity)
{
    QList<int> IDs;
    foreach (NotificationObject* obj, notificationObjects.values()) {
        if (obj->severity() == severity) {
            IDs.append(obj->ID());
        }
    }
    return IDs;
}


/**
 * @brief NotificationManager::getNotificationsOfCategory
 * @param category
 * @return
 */
QList<int> NotificationManager::getNotificationsOfCategory(NOTIFICATION_CATEGORY category)
{
    QList<int> IDs;
    foreach (NotificationObject* obj, notificationObjects.values()) {
        if (obj->category() == category) {
            IDs.append(obj->ID());
        }
    }
    return IDs;
}


/**
 * @brief NotificationManager::clearModelNotifications
 */
void NotificationManager::clearModelNotifications()
{
    // only clear NOTIFICATION_TYPE::MODEL notifications
    foreach (int ID, getNotificationsOfType(NOTIFICATION_TYPE::MODEL)) {
        deleteNotification(ID);
    }
}


/**
 * @brief NotificationManager::displayNotification
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 * @param s
 * @param t
 * @param c
 * @return
 */
int NotificationManager::displayNotification(QString description, QString iconPath, QString iconName, int entityID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE t, NOTIFICATION_CATEGORY c)
{
    if (managerSingleton) {
        return managerSingleton->addNotification(description, iconPath, iconName, entityID, s, t, c);
    }
    return -1;
}


/**
 * @brief NotificationManager::displayLoadingNotification
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 * @param s
 * @param t
 * @param c
 * @return
 */
int NotificationManager::displayLoadingNotification(QString description, QString iconPath, QString iconName, int entityID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE t, NOTIFICATION_CATEGORY c)
{
    int ID = displayNotification(description, iconPath, iconName, entityID, s, t, c);
    setNotificationLoading(ID, true);
    return ID;
}

void NotificationManager::popupLatestNotification(){
    auto popup = getNotificationPopup();
    auto latest_notification = getLastNotificationItem();
    if(popup && latest_notification){
        popup->DisplayNotification(latest_notification);
        popup->show();
        WindowManager::MoveWidget(popup, 0, Qt::AlignBottom);
    }
}

void NotificationManager::centerPopup(){

    WindowManager::MoveWidget(getNotificationPopup(), 0, Qt::AlignBottom);
    getNotificationPopup()->show();
}


NotificationPopup* NotificationManager::getNotificationPopup(){
    if(!notification_popup){
        notification_popup = new NotificationPopup();
        connect(this, &NotificationManager::notificationAdded, this, &NotificationManager::popupLatestNotification);
    }
    return notification_popup;
}

/**
 * @brief NotificationManager::updateNotification
 * @param ID
 * @param description
 * @param iconPath
 * @param iconName
 * @param severity
 * @return
 */
bool NotificationManager::updateNotification(int ID, QString description, QString iconPath, QString iconName, NOTIFICATION_SEVERITY severity)
{
    if (!notificationObjects.contains(ID)) {
        return false;
    }

    NotificationObject* obj = notificationObjects.value(ID);
    obj->setDescription(description);
    obj->setIcon(iconPath, iconName);
    obj->setSeverity(severity);

    // toast updated notification
    emit manager()->notificationAdded(iconPath, iconName, description);

    return true;
}


/**
 * @brief NotificationManager::setNotificationLoading
 * @param ID
 * @param on
 * @return
 */
bool NotificationManager::setNotificationLoading(int ID, bool on)
{
    if (!notificationObjects.contains(ID)) {
        return false;
    }

    NotificationObject* obj = notificationObjects.value(ID);
    obj->setLoading(on);
    return true;
}


/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * Send a signal to update the warning/error count in the notification toolbar.
 * @param ID
 */
void NotificationManager::deleteNotification(int ID)
{
    if (!notificationObjects.contains(ID)) {
        qWarning() << "NotificationManager::deleteNotification - Requesting to delete a notification item that's not contained in the hash.";
        return;
    }

    // if the deleted notification is the top-most currently, update the lastNotification object
    bool topNotificationDeleted = false;
    if (lastNotificationObject && (lastNotificationObject->ID() == ID)) {
        lastNotificationObject = 0;
        topNotificationDeleted = true;
        emit notificationSeen();
    }

    // delete the item from the notification dialog
    emit notificationDeleted(ID);

    // remove item from hash before deleting it and update the severity count hash
    NotificationObject* obj = notificationObjects.take(ID);
    updateSeverityCountHash(obj->severity(), false);
    delete obj;

    if (notificationObjects.isEmpty()) {
        emit lastNotificationDeleted();
    } else {
        if (topNotificationDeleted) {
            // request the new top-most notification's ID
            emit req_lastNotificationID();
        }
    }
}


/**
 * @brief NotificationManager::setLastNotificationItem
 * @param item
 */
void NotificationManager::setLastNotificationItem(int ID)
{
    lastNotificationObject = notificationObjects.value(ID, 0);

    // if there is no last notification, disable the showMostRecentNotification button in the toolbar
    if (!lastNotificationObject) {
        emit lastNotificationDeleted();
    }
}

NotificationObject* NotificationManager::getNotificationItem(int id){
    return notificationObjects.value(id, 0);
}
NotificationObject* NotificationManager::getLastNotificationItem(){
    return lastNotificationObject;
}




/**
 * @brief NotificationManager::activeSelectionChanged
 * @param item
 * @param isActive
 */
void NotificationManager::activeSelectionChanged(ViewItem* item, bool isActive)
{
    if (!item || !isActive) {
        emit selectionChanged(-1);
    } else {
        emit selectionChanged(item->getID());
    }
}


/**
 * @brief NotificationManager::addNotification
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 * @param s
 * @param t
 * @param c
 * @param toast
 * @return
 */
int NotificationManager::addNotification(QString description, QString iconPath, QString iconName, int entityID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE t, NOTIFICATION_CATEGORY c, bool toast)
{
    // construct notification item
    NotificationObject* obj = new NotificationObject("", description, iconPath, iconName, entityID, s, t, c, 0);
    notificationObjects[obj->ID()] = obj;
    lastNotificationObject = obj;

    // send signal to the notifications widget; highlight showMostRecentNotification button
    emit notificationAlert();

    // send signal to the notification dialog; add notification item
    emit notificationItemAdded(obj);

    // send signal to main window; display notification toast
    if (toast) {
        emit notificationAdded(iconPath, iconName, description);
    }

    // update severity count
    updateSeverityCountHash(s, true);
    return obj->ID();
}


/**
 * @brief NotificationManager::updateSeverityCountHash
 * Update severity count hash and send a signal to update the displayed severity count in the notification display toolbar(s).
 * @param severity
 * @param increment
 */
void NotificationManager::updateSeverityCountHash(NOTIFICATION_SEVERITY severity, bool increment)
{
    int count = severityCount.value(severity, 0);
    if (increment) {
        count = count + 1;
    } else {
        count = qMax(count - 1, 0);
    }
    severityCount[severity] = count;
    emit updateSeverityCount(severity, count);
}
