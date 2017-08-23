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
    auto theme = Theme::theme();

    QString notification_str("Notification");
    QString icon_prefix("Icons");

    //Setup Context Icons
    theme->setIconAlias(notification_str, Notification::getContextString(Notification::Context::NOT_SELECTED), icon_prefix, "tiles");
    theme->setIconAlias(notification_str, Notification::getContextString(Notification::Context::SELECTED), icon_prefix, "tiles");
    
    //Setup Severity Icons
    theme->setIconAlias(notification_str, Notification::getSeverityString(Notification::Severity::INFO), icon_prefix, "circleInfoDark");
    theme->setIconAlias(notification_str, Notification::getSeverityString(Notification::Severity::WARNING), icon_prefix, "triangleCritical");
    theme->setIconAlias(notification_str, Notification::getSeverityString(Notification::Severity::ERROR), icon_prefix, "pointyCircleCriticalDark");

    //Setup Category Icons
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::JENKINS), icon_prefix, "jenkinsFlat");
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::FILE), icon_prefix, "file");
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::VALIDATION), icon_prefix, "shieldTick");
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::NONE), icon_prefix, "tiles");

    //Setup Type Icons
    theme->setIconAlias(notification_str, Notification::getTypeString(Notification::Type::MODEL), icon_prefix, "dotsInRectangle");
    theme->setIconAlias(notification_str, Notification::getTypeString(Notification::Type::APPLICATION), icon_prefix, "circleQuestion");
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
    NotificationDialog* panel = new NotificationDialog(manager()->viewController);
    
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
QList<int> NotificationManager::getNotificationsOfType(Notification::Type type)
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
QList<int> NotificationManager::getNotificationsOfSeverity(Notification::Severity severity)
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
QList<int> NotificationManager::getNotificationsOfCategory(Notification::Category category)
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
    // only clear Notification::Type::MODEL notifications
    foreach (int ID, getNotificationsOfType(Notification::Type::MODEL)) {
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
int NotificationManager::displayNotification(QString description, QString iconPath, QString iconName, int entityID, Notification::Severity s, Notification::Type t, Notification::Category c)
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
int NotificationManager::displayLoadingNotification(QString description, QString iconPath, QString iconName, int entityID, Notification::Severity s, Notification::Type t, Notification::Category c)
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
bool NotificationManager::updateNotification(int ID, QString description, QString iconPath, QString iconName, Notification::Severity severity)
{
    if (!notificationObjects.contains(ID)) {
        return false;
    }

    NotificationObject* obj = notificationObjects.value(ID);
    obj->setDateTime(QDateTime::currentDateTime());
    obj->setDescription(description);
    obj->setIcon(iconPath, iconName);
    obj->setSeverity(severity);
    obj->setLoading(false);

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
int NotificationManager::addNotification(QString description, QString iconPath, QString iconName, int entityID, Notification::Severity s, Notification::Type t, Notification::Category c, bool toast)
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
void NotificationManager::updateSeverityCountHash(Notification::Severity severity, bool increment)
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
