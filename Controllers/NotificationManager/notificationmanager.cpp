#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/mainwindow.h"


/**
 * @brief NotificationManager::NotificationManager
 * @param vc
 * @param parent
 */
NotificationManager::NotificationManager(ViewController *vc, QObject *parent) : QObject(parent)
{
    viewController = vc;
    toggleNotificationsDialog = vc->getActionController()->window_showNotifications;
    notificationDialog = new NotificationDialog((QWidget*)parent);
    //notificationDialog = new NotificationDialog(WindowManager::manager()->getMainWindow());

    connect(viewController, &ViewController::vc_showNotification, this, &NotificationManager::notificationReceived);
    connect(viewController, &ViewController::vc_setupModel, notificationDialog, &NotificationDialog::resetDialog);
    connect(toggleNotificationsDialog, &QAction::triggered, notificationDialog, &NotificationDialog::toggleVisibility);
    connect(toggleNotificationsDialog, &QAction::triggered, this, &NotificationManager::notificationsSeen);
    connect(notificationDialog, &NotificationDialog::itemDeleted, this, &NotificationManager::deleteNotification);
    connect(notificationDialog, &NotificationDialog::centerOn, viewController, &ViewController::centerOnID);
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
        // TODO - At the moment, some (or all?) notifications with the same type share the same ID
        //return;
    }

    // store notification in hash
    Notification n;
    n.type = type;
    n.title = title;
    n.description = description;
    n.iconPath = iconPath;
    n.iconName = iconName;
    n.ID = ID;
    notifications[ID] = n;

    // add notification to dialog
    notificationDialog->addNotificationItem(type, title, description, QPair<QString, QString>(iconPath, iconName), ID);

    // highlight notification dialog toggle
    if (!toggleNotificationsDialog->isCheckable()) {
        toggleNotificationsDialog->setCheckable(true);
        toggleNotificationsDialog->setChecked(true);
    }

    // send signal to main window to display notification toast
    emit notificationAdded(iconPath, iconName, description);
}


/**
 * @brief NotificationManager::notificationsSeen
 * Remove highlight from the notification dialog's toggle button.
 */
void NotificationManager::notificationsSeen()
{
    if (toggleNotificationsDialog->isCheckable()) {
        toggleNotificationsDialog->setCheckable(false);
        toggleNotificationsDialog->setChecked(false);
    }
}


/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * @param ID
 */
void NotificationManager::deleteNotification(int ID)
{
    if (notifications.contains(ID)) {
        // if the sender object is the dialog, it means that the item has already been removed from it
        if (QObject::sender() != notificationDialog) {
            notificationDialog->removeNotificationItem(ID);
        }
        notifications.remove(ID);
    }
}

