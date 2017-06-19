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
 * @brief NotificationManager::NotificationManager
 * @param controller
 */
NotificationManager::NotificationManager(ViewController* controller)
{
    qRegisterMetaType<BACKGROUND_PROCESS>("BACKGROUND_PROCESS");
    viewController = controller;
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
    connect(manager(), &NotificationManager::backgroundProcess, panel, &NotificationDialog::backgroundProcess);
    connect(manager(), &NotificationManager::notificationItemAdded, panel, &NotificationDialog::notificationAdded);
    connect(manager(), &NotificationManager::notificationDeleted, panel, &NotificationDialog::notificationDeleted);
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
 * @brief NotificationManager::clearModelNotifications
 */
void NotificationManager::clearModelNotifications()
{
    // TODO - Optimise
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
 * @brief NotificationManager::updateNotification
 * @param ID
 * @param iconPath
 * @param iconName
 * @param description
 * @return
 */
bool NotificationManager::updateNotification(int ID, QString iconPath, QString iconName, QString description)
{
    // TODO - add modifiable parameters
    // allow change of icon and change of text

    if (!notificationObjects.contains(ID)) {
        return false;
    }

    NotificationObject* obj = notificationObjects.value(ID);
    obj->setIconPath(iconPath);
    obj->setIconName(iconName);
    obj->setDescription(description);

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
