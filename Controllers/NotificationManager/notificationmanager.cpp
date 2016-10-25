#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/mainwindow.h"


NotificationManager* NotificationManager::managerSingleton = 0;
int NotificationManager::_NotificationID  = 0;


/**
 * @brief NotificationManager::manager
 * @return
 */
NotificationManager* NotificationManager::manager()
{
    if (!managerSingleton) {
        //managerSingleton = new NotificationManager();
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
 * @brief NotificationManager::NotificationManager
 * @param vc
 * @param parent
 */
NotificationManager::NotificationManager(ViewController *vc, QWidget *parent) : QObject(parent)
{
    viewController = vc;
    notificationDialog = new NotificationDialog(parent);
}


/**
 * @brief NotificationManager::notificationReceived
 * @param type
 * @param title
 * @param description
 * @param iconPath
 * @param iconName
 * @param ID
 */
void NotificationManager::notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int ID)
{
    if (notifications.contains(ID)) {
        return;
    }

    // store notification in hash
    Notification n;
    n.type = type;
    n.title = title;
    n.description = description;
    n.iconPath = iconPath;
    n.iconName = iconName;
    n.entityID = ID;
    n.ID = ++_NotificationID;

    notifications[n.ID] = n;
    lastNotification = n;

    // add notification to dialog
    notificationDialog->addNotificationItem(n.ID, type, title, description, QPair<QString, QString>(iconPath, iconName), ID);

    // send signal to main window to display notification toast
    emit notificationAdded(iconPath, iconName, description);
}


/**
 * @brief NotificationManager::showLastNotification
 * Send a signal to the main window to show the last notification (last item in the dialog's list).
 */
void NotificationManager::showLastNotification()
{
    if (!notifications.isEmpty()) {
        emit notificationAdded(lastNotification.iconPath, lastNotification.iconName, lastNotification.description);
    }
}


/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * @param ID
 */
void NotificationManager::deleteNotification(int ID)
{
    // if the sender object is not the dialog, remove item from dialog
    if (sender() != notificationDialog) {
        notificationDialog->removeNotificationItem(ID);
        return;
    }

    // remove from hash
    notifications.remove(ID);

    // check if the deleted notification is the last notification
    if (lastNotification.ID == ID) {
        int topNotificationID = notificationDialog->getTopNotificationID();
        if (topNotificationID == -1) {
            // if top ID is -1, it means that the notifications list is empty
            emit lastNotificationDeleted();
        } else {
            // remove highlight from the button when the last notification is deleted
            emit notificationSeen();
            lastNotification = notifications.value(topNotificationID);
        }
    }
}


