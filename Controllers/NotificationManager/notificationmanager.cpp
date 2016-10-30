#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationitem.h"


NotificationManager* NotificationManager::managerSingleton = 0;


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
}


/**
 * @brief NotificationManager::showLastNotification
 * Send a signal to the main window to show the last notification (last item in the dialog's list).
 */
void NotificationManager::showLastNotification()
{
    if (!notificationItems.isEmpty()) {
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
    if (lastNotificationItem->ID() == ID) {
        int topNotificationID = notificationDialog->getTopNotificationID();
        if (topNotificationID == -1) {
            // if top ID is -1, it means that the notifications list is empty
            emit lastNotificationDeleted();
        } else {
            // remove highlight from the button when the last notification is deleted
            emit notificationSeen();
            lastNotificationItem = notificationItems.value(topNotificationID);
        }
    }
}


