#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationobject.h"
#include "../ViewController/viewcontroller.h"

NotificationManager* NotificationManager::managerSingleton = 0;
NotificationObject* NotificationManager::lastNotificationObject = 0;

QTime* NotificationManager::projectRunTime = new QTime();
QHash<int, NotificationObject*> NotificationManager::notificationObjects;

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
 * @brief NotificationManager::projectTime
 * @return
 */
QTime* NotificationManager::projectTime()
{
    return projectRunTime;
}


/**
 * @brief NotificationManager::resetManager
 */
void NotificationManager::resetManager()
{
    // only clear NT_MODEL notifications
    QList<NotificationObject*> modelNotifications;
    foreach (NotificationObject* obj, getNotificationItems()) {
        if (obj->type() == NT_MODEL) {
            modelNotifications.append(obj);
        }
    }
    foreach (NotificationObject* m_obj, modelNotifications) {
        deleteNotification(m_obj->ID());
    }

    projectRunTime->restart();

    //emit notificationSeen();
    emit showNotificationDialog(false);
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
 * @brief NotificationManager::displayNotification
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 * @param s
 * @param t
 * @param c
 */
void NotificationManager::displayNotification(QString description, QString iconPath, QString iconName, int entityID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE2 t, NOTIFICATION_CATEGORY c)
{
    addNotification(description, iconPath, iconName, entityID, s, t, c);
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
 * @brief NotificationManager::getBackgroundProcesses
 * @return
 */
QList<BACKGROUND_PROCESS> NotificationManager::getBackgroundProcesses()
{
    QList<BACKGROUND_PROCESS> processes;
    processes.append(BP_UNKNOWN);
    processes.append(BP_VALIDATION);
    processes.append(BP_IMPORT_JENKINS);
    return processes;
}


/**
 * @brief NotificationManager::getNotificationFilters
 * @return
 */
QList<NOTIFICATION_FILTER> NotificationManager::getNotificationFilters()
{
    QList<NOTIFICATION_FILTER> filters;
    filters.append(NF_NOFILTER);
    filters.append(NF_SEVERITY);
    filters.append(NF_TYPE);
    filters.append(NF_CATEGORY);
    return filters;
}


/**
 * @brief NotificationManager::getNotificationTypes
 * @return
 */
QList<NOTIFICATION_TYPE2> NotificationManager::getNotificationTypes()
{
    QList<NOTIFICATION_TYPE2> types;
    types.append(NT_MODEL);
    types.append(NT_APPLICATION);
    return types;
}


/**
 * @brief NotificationManager::getNotificationCategories
 * @return
 */
QList<NOTIFICATION_CATEGORY> NotificationManager::getNotificationCategories()
{
    QList<NOTIFICATION_CATEGORY> categories;
    categories.append(NC_NOCATEGORY);
    categories.append(NC_DEPLOYMENT);
    categories.append(NC_FILE);
    categories.append(NC_JENKINS);
    categories.append(NC_VALIDATION);
    return categories;
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
 * @brief NotificationManager::getTypeString
 * @param type
 * @return
 */
QString NotificationManager::getTypeString(NOTIFICATION_TYPE2 type)
{
    switch (type) {
    case NT_MODEL:
        return "Model";
    case NT_APPLICATION:
        return "Application";
    default:
        return "Unknown Type";
    }
}


/**
 * @brief NotificationManager::getCategoryString
 * @param category
 * @return
 */
QString NotificationManager::getCategoryString(NOTIFICATION_CATEGORY category)
{
    switch (category) {
    case NC_DEPLOYMENT:
        return "Deployment";
    case NC_FILE:
        return "File";
    case NC_JENKINS:
        return "Jenkins";
    case NC_VALIDATION:
        return "Validation";
    default:
        return "No Category";
    }
}


/**
 * @brief NotificationManager::getSeverityString
 * @param severity
 * @return
 */
QString NotificationManager::getSeverityString(NOTIFICATION_SEVERITY severity)
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
 * @brief NotificationManager::getSeverityColor
 * @param severity
 * @return
 */
QColor NotificationManager::getSeverityColor(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NS_INFO:
        return QColor(0,180,180);
    case NS_WARNING:
        return QColor(255,200,0);
    case NS_ERROR:
        return QColor(255,50,50);
    default:
        return Qt::white;
    }
}


/**
 * @brief NotificationManager::getSeverityColorStr
 * @param severity
 * @return
 */
QString NotificationManager::getSeverityColorStr(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NS_INFO:
        return "rgb(0,180,180)";
    case NS_WARNING:
        return "rgb(255,200,0)";
    case NS_ERROR:
        return "rgb(255,50,50)";
    default:
        return "white";
    }
}


/**
 * @brief NotificationManager::getSeverityIcon
 * @param severity
 * @return
 */
QPair<QString, QString> NotificationManager::getSeverityIcon(NOTIFICATION_SEVERITY severity)
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
 * @brief NotificationManager::showLastNotification
 * Send a signal to the main window to show the last notification (last item in the dialog's list).
 */
void NotificationManager::showLastNotification()
{
    if (lastNotificationObject) {
        emit notificationAdded(lastNotificationObject->iconPath(), lastNotificationObject->iconName(), lastNotificationObject->description());
    }
}


/**
 * @brief NotificationManager::modelValidated
 * @param report
 */
void NotificationManager::modelValidated(QStringList report)
{
    QString status = "Failed";
    if (report.isEmpty()) {
        status = "Succeeded";
    } else {
        foreach (QString message, report) {
            QString description = message.split(']').last().trimmed();
            QString eID = message.split('[').last().split(']').first();
            addNotification(description, "", "", eID.toInt(), NS_WARNING, NT_MODEL, NC_VALIDATION, false);
        }
        emit showNotificationDialog();
    }
    addNotification("Model Validation " + status, "Actions", "Validate", -1, NS_INFO, NT_MODEL, NC_VALIDATION);
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
 */
void NotificationManager::addNotification(QString description, QString iconPath, QString iconName, int entityID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE2 t, NOTIFICATION_CATEGORY c, bool toast)
{
    // construct notification item
    NotificationObject* item = new NotificationObject("", description, iconPath, iconName, entityID, s, t, c, 0);
    notificationObjects[item->ID()] = item;
    lastNotificationObject = item;

    // send signal to the notifications widget; highlight showMostRecentNotification button
    emit notificationAlert();

    // send signal to the notification dialog; add notification item
    emit notificationItemAdded(item);

    // send signal to main window; display notification toast
    if (toast) {
        emit notificationAdded(iconPath, iconName, description);
    }
}


/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * Send a signal to update the warning/error counbt in the notification toolbar.
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

    // send a signal to update the notification toolbar's severity count and to delete the item
    // from the notification dialog, then remove item from hash before deleting it
    emit notificationDeleted(ID);
    delete notificationObjects.take(ID);

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


