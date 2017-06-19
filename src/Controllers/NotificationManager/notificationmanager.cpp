#include "notificationmanager.h"
#include "../WindowManager/windowmanager.h"
#include "../ViewController/viewcontroller.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationobject.h"
#include "../../Views/Notification/notificationdialog.h"
#include "../../Views/Notification/notificationtoolbar.h"
#include "../../Widgets/Dialogs/popupwidget.h"

NotificationManager* NotificationManager::managerSingleton = 0;
NotificationObject* NotificationManager::lastNotificationObject = 0;

QHash<int, NotificationObject*> NotificationManager::notificationObjects;

/**
 * @brief NotificationManager::manager
 * @return
 */
NotificationManager* NotificationManager::manager()
{
    if (!managerSingleton) {
        qRegisterMetaType<BACKGROUND_PROCESS>("BACKGROUND_PROCESS");
        managerSingleton = new NotificationManager();
    }
    return managerSingleton;
}


/**
 * @brief NotificationManager::displayPanel
 * @return
 */
NotificationDialog* NotificationManager::displayPanel()
{
    NotificationDialog* panel = new NotificationDialog();
    connect(manager(), &NotificationManager::req_lastNotificationID, panel, &NotificationDialog::getLastNotificationID);
    connect(manager(), &NotificationManager::backgroundProcess, panel, &NotificationDialog::backgroundProcess);
    connect(manager(), &NotificationManager::notificationItemAdded, panel, &NotificationDialog::notificationAdded);
    connect(manager(), &NotificationManager::notificationDeleted, panel, &NotificationDialog::notificationDeleted);
    connect(panel, &NotificationDialog::deleteNotification, manager(), &NotificationManager::deleteNotification);
    connect(panel, &NotificationDialog::lastNotificationID, manager(), &NotificationManager::setLastNotificationItem);
    connect(panel, &NotificationDialog::mouseEntered, manager(), &NotificationManager::notificationSeen);
    return panel;
}


/**
 * @brief NotificationManager::displayToolbar
 * @return
 */
NotificationToolbar* NotificationManager::displayToolbar()
{
    NotificationToolbar* toolbar = new NotificationToolbar();
    connect(manager(), &NotificationManager::backgroundProcess, toolbar, &NotificationToolbar::displayLoadingGif);
    connect(manager(), &NotificationManager::notificationAlert, toolbar, &NotificationToolbar::notificationReceived);
    connect(manager(), &NotificationManager::notificationSeen, toolbar, &NotificationToolbar::notificationsSeen);
    connect(manager(), &NotificationManager::updateSeverityCount, toolbar, &NotificationToolbar::updateSeverityCount);
    connect(manager(), &NotificationManager::lastNotificationDeleted, toolbar, &NotificationToolbar::lastNotificationDeleted);
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
 * @brief NotificationManager::getBackgroundProcesses
 * @return
 */
QList<BACKGROUND_PROCESS> NotificationManager::getBackgroundProcesses()
{
    QList<BACKGROUND_PROCESS> processes;
    processes.append(BP_UNKNOWN);
    processes.append(BP_VALIDATION);
    processes.append(BP_IMPORT_JENKINS);
    processes.append(BP_RUNNING_JOB);
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
QList<NOTIFICATION_TYPE> NotificationManager::getNotificationTypes()
{
    QList<NOTIFICATION_TYPE> types;
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
QString NotificationManager::getTypeString(NOTIFICATION_TYPE type)
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
 * @brief NotificationManager::getSeverityIcon
 * @param severity
 * @return
 */
QString NotificationManager::getSeverityIcon(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NS_INFO:
        return "circleInfoDark";
    case NS_WARNING:
        return "triangleCritical";
    case NS_ERROR:
        return "circleCrossDark";
    default:
        return "circleQuestion";
    }
}


/**
 * @brief NotificationManager::getCategoryIcon
 * @param category
 * @return
 */
QString NotificationManager::getCategoryIcon(NOTIFICATION_CATEGORY category)
{
    switch (category) {
    case NC_DEPLOYMENT:
        return "screen";
    case NC_JENKINS:
        return "jenkins";
    case NC_FILE:
        return "file";
    case NC_VALIDATION:
        return "shieldTick";
    default:
        return "tiles";
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
    // remove model notifications from the hash and the notification dialog
    foreach (NotificationObject* m_obj, modelNotifications) {
        deleteNotification(m_obj->ID());
    }
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
void NotificationManager::displayNotification(QString description, QString iconPath, QString iconName, int entityID, NOTIFICATION_SEVERITY s, NOTIFICATION_TYPE t, NOTIFICATION_CATEGORY c)
{
    addNotification(description, iconPath, iconName, entityID, s, t, c);
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


/**
 * @brief NotificationManager::showLastNotification
 * Send a signal to the main window to toast the last notification (last item in the dialog's list).
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
void NotificationManager::modelValidated(QString report)
{
    // Clear previous model validation items before re-validating
    QListIterator<NotificationObject*> it(notificationObjects.values());
    while (it.hasNext()) {
        NotificationObject* obj = it.next();
        if (obj->category() == NC_VALIDATION) {
            deleteNotification(obj->ID());
        }
    }

    QString status = "Failed";
    if (report.isEmpty()) {
        status = "Succeeded";
    } else {
        foreach (QString message, report) {
            QString description = message.split(']').last().trimmed();
            QString eID = message.split('[').last().split(']').first();
            addNotification(description, "", "", eID.toInt(), NS_WARNING, NT_MODEL, NC_VALIDATION, false);
        }
        emit showNotificationPanel();
        emit updateNotificationToolbarSize();
    }
    addNotification("Model Validation " + status, "Icons", "shieldTick", -1, NS_INFO, NT_MODEL, NC_VALIDATION);
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

    // update severity count
    updateSeverityCountHash(s, true);
    return item->ID();
}


/**
 * @brief NotificationManager::updateNotification
 * @param ID
 * @return
 */
bool NotificationManager::updateNotification(int ID /*, modifiable params*/)
{
    // TODO - add modifiable parameters
    return true;
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
