#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationobject.h"
#include "../ViewController/viewcontroller.h"

NotificationManager* NotificationManager::managerSingleton = 0;
NotificationObject* NotificationManager::lastNotificationItem = 0;

QTime* NotificationManager::projectRunTime = new QTime();
QHash<int, NotificationObject*> NotificationManager::notificationItems;

/**
 * @brief NotificationManager::manager
 * @return
 */
NotificationManager* NotificationManager::manager()
{
    if (!managerSingleton) {
        managerSingleton = new NotificationManager();
        projectRunTime->start();
    }
    return managerSingleton;
}


/**
 * @brief NotificationManager::resetManager
 */
void NotificationManager::resetManager()
{
    projectRunTime->restart();
    lastNotificationItem = 0;
    notificationItems.clear();
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
    resetManager();
}


/**
 * @brief NotificationManager::projectTime
 * @return
 */
QTime* NotificationManager::projectTime()
{
    return projectRunTime;
}


/**
 * @brief NotificationManager::getNotificationItems
 * @return
 */
QList<NotificationObject*> NotificationManager::getNotificationItems()
{
    return notificationItems.values();
}


/**
 * @brief NotificationManager::getNotificationSeverities
 * @return
 */
QList<NOTIFICATION_SEVERITY> NotificationManager::getNotificationSeverities()
{
    QList<NOTIFICATION_SEVERITY> severities;
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
 * @brief NotificationManager::newNotification
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 * @param s
 * @param t
 * @param c
 */
void NotificationManager::newNotification(QString description, QString iconPath, QString iconName, int entityID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE2 t, NOTIFICATION_CATEGORY c)
{
    // construct notification item
    NotificationObject* item = new NotificationObject("", description, iconPath, iconName, entityID, s, t, c, this);
    notificationItems[item->ID()] = item;
    lastNotificationItem = item;

    // send signal to the notifications widget; highlight showMostRecentNotification button
    emit notificationAlert();

    // send signal to the notification dialog; add notification item
    emit notificationItemAdded(item);

    // send signal to main window; display notification toast
    emit notificationAdded(iconPath, iconName, description);
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
    // TODO - This can be removed when we delete the NOTIFICATION_TYPE enum from enumerations
    NOTIFICATION_SEVERITY s = NS_INFO;
    if (type != NT_INFO) {
        switch(type) {
        case NT_WARNING:
            s = NS_WARNING;
            break;
        case NT_ERROR:
            s = NS_ERROR;
            break;
        default:
             break;
        }
    }

    // construct notification item
    NotificationObject* item = new NotificationObject(title, description, iconPath, iconName, entityID, s, NT_MODEL, NC_NOCATEGORY, this);
    notificationItems[item->ID()] = item;
    lastNotificationItem = item;

    // send signal to the notifications widget; highlight showMostRecentNotification button
    emit notificationAlert();

    // send signal to the notification dialog; add notification item
    emit notificationItemAdded(item);

    // send signal to main window; display notification toast
    emit notificationAdded(iconPath, iconName, description);
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
 * @brief NotificationManager::modelValidated
 * @param report
 */
void NotificationManager::modelValidated(QStringList report)
{
    foreach (QString message, report) {
        QString description = message.split(']').last();
        QString eID = message.split('[').last().split(']').first();
        notificationReceived(NT_WARNING, "Model Validation", description, "", "", eID.toInt());
        //newNotification(description, "", "", eID.toInt(), NS_WARNING, NT_MODEL, NC_VALIDATION);
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


