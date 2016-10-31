#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationitem.h"


NotificationManager* NotificationManager::managerSingleton = 0;
NotificationItem* NotificationManager::lastNotificationItem = 0;
QHash<int, NotificationItem*> NotificationManager::notificationItems;


/**
 * @brief NotificationManager::manager
 * @return
 */
NotificationManager* NotificationManager::manager()
{
    if (!managerSingleton) {
        managerSingleton = new NotificationManager();
    }
    return managerSingleton;
}


/**
 * @brief NotificationManager::tearDown
 */
void NotificationManager::tearDown()
{
    if (managerSingleton) {
        delete managerSingleton;
    }
    managerSingleton = 0;
    lastNotificationItem = 0;
}


/**
 * @brief NotificationManager::getNotificationItems
 * @return
 */
QList<NotificationItem*> NotificationManager::getNotificationItems()
{
    return notificationItems.values();
}


/**
 * @brief NotificationManager::getNotificationSeverities
 * @return
 */
QList<NotificationManager::NOTIFICATION_SEVERITY> NotificationManager::getNotificationSeverities()
{
    QList<NotificationManager::NOTIFICATION_SEVERITY> severities;
    severities.append(NS_INFO);
    severities.append(NS_WARNING);
    severities.append(NS_ERROR);
    return severities;
}


/**
 * @brief NotificationManager::getNotificationSeverityString
 * @param s
 * @return
 */
QString NotificationManager::getNotificationSeverityString(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NS_INFO:
        return "Information";
    case NS_WARNING:
        return "Warning";
    case NS_ERROR:
        return "Error";
    default:
        return "Unknown Severity";
    }
}


/**
 * @brief NotificationManager::getNotificationSeverityColorStr
 * @param severity
 * @return
 */
QString NotificationManager::getNotificationSeverityColorStr(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NS_WARNING:
        return "rgb(255,200,0)";
    case NS_ERROR:
        return "rgb(255,50,50)";
    default:
        return "white";
    }
}


/**
 * @brief NotificationManager::notificationReceived
 * @param type
 * @param title
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 */
void NotificationManager::notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int entityID)
{
    // construct notification item
    NotificationItem* item = new NotificationItem(title, description, iconPath, iconName, entityID, NT_MODEL, NC_NONE, NS_INFO, this);
    notificationItems[item->ID()] = item;
    lastNotificationItem = item;

    // send signal to notification dialog
    emit notificationItemAdded(item);

    // send signal to main window to display notification toast
    emit notificationAdded(iconPath, iconName, description);

    qDebug() << "Notification Received: " << description;
}


/**
 * @brief NotificationManager::showLastNotification
 * Send a signal to the main window to show the last notification (last item in the dialog's list).
 */
void NotificationManager::showLastNotification()
{
    if (lastNotificationItem) {
        emit notificationAdded(lastNotificationItem->iconPath(), lastNotificationItem->iconName(), lastNotificationItem->description());
    }
}


/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * @param ID
 */
void NotificationManager::deleteNotification(int ID)
{
    // remove from hash
    notificationItems.remove(ID);

    // check if the deleted notification is the last notification
    if (lastNotificationItem && lastNotificationItem->ID() == ID) {
        if (notificationItems.isEmpty()) {
            // if the notifications list is empty, disable showMostRectNotification button
            emit lastNotificationDeleted();
            lastNotificationItem = 0;
        } else {
            // remove highlight from the showMostRectNotification button and update lastNotificationItem
            emit req_lastNotificationID();
            emit notificationSeen();
        }
    }
}


/**
 * @brief NotificationManager::setLastNotificationItem
 * @param item
 */
void NotificationManager::setLastNotificationItem(int ID)
{
    lastNotificationItem = notificationItems.value(ID, 0);
}


