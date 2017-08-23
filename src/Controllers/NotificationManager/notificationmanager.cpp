#include "notificationmanager.h"
#include "notificationobject.h"

#include "../WindowManager/windowmanager.h"
#include "../ViewController/viewcontroller.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationdialog.h"
#include "../../Views/Notification/notificationtoolbar.h"
#include "../../Views/Notification/notificationpopup.h"
#include "../../Widgets/Dialogs/popupwidget.h"

NotificationManager* NotificationManager::managerSingleton = 0;

/**
 * @brief NotificationManager::NotificationManager
 * @param controller
 */
NotificationManager::NotificationManager(ViewController* controller)
{
    viewController = controller;
    getToast();
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


NotificationManager::~NotificationManager(){
    //TODO Clear up memory
}

bool NotificationManager::construct_singleton(ViewController* controller)
{
    if (!managerSingleton) {
        managerSingleton = new NotificationManager(controller);
        return true;
    }
    return false;
}

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



NotificationDialog* NotificationManager::getPanel()
{
    if(!notification_panel){
        notification_panel = new NotificationDialog(viewController);
    }
    return notification_panel;
}

NotificationPopup* NotificationManager::getToast()
{
    if(!notification_popup){
        notification_popup = new NotificationPopup();
        connect(this, &NotificationManager::toastNotification, [=](NotificationObject* notification){
            if(!viewController->isWelcomeScreenShowing()){
                notification_popup->DisplayNotification(notification);
                WindowManager::MoveWidget(notification_popup, 0, Qt::AlignBottom);
                notification_popup->show();
            }
        });
    }
    return notification_popup;
}

void NotificationManager::hideToast(){
    getToast()->hide();
}

NotificationToolbar* NotificationManager::getToolbar()
{
    if(!notification_toolbar){
        notification_toolbar = new NotificationToolbar();
    }
    return notification_toolbar;
}

/**
 * @brief NotificationManager::getNotificationItems
 * @return
 */
QList<NotificationObject*> NotificationManager::getNotifications()
{
    return notifications.values();
}


/**
 * @brief NotificationManager::getNotificationsOfType
 * @param type
 * @return
 */
QList<NotificationObject*> NotificationManager::getNotificationsOfType(Notification::Type type)
{
    QList<NotificationObject*> list;
    for (auto notification : notifications.values()) {
        if (notification->getType() == type) {
            list << notification;
        }
    }
    return list;
}


/**
 * @brief NotificationManager::getNotificationsOfSeverity
 * @param severity
 * @return
 */
QList<NotificationObject*> NotificationManager::getNotificationsOfSeverity(Notification::Severity severity)
{
    QList<NotificationObject*> list;
    for (auto notification : notifications.values()) {
        if (notification->getSeverity() == severity) {
            list << notification;
        }
    }
    return list;
}


/**
 * @brief NotificationManager::getNotificationsOfCategory
 * @param category
 * @return
 */
QList<NotificationObject*> NotificationManager::getNotificationsOfCategory(Notification::Category category)
{
    QList<NotificationObject*> list;
    for (auto notification : notifications.values()) {
        if (notification->getCategory() == category) {
            list << notification;
        }
    }
    return list;
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

void NotificationManager::toastLatestNotification(){
    auto popup = getToast(); 

    if(latest_notification){
        emit toastNotification(latest_notification);
    }
    
    
}

void NotificationManager::centerPopup(){
    auto popup = getToast();
    WindowManager::MoveWidget(popup, 0, Qt::AlignBottom);
    popup->show();
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
    auto notification = manager()->notifications.value(ID, 0);
    if(notification){
        notification->setDescription(description);
        notification->setIcon(iconPath, iconName);
        notification->setSeverity(severity);
        notification->setInProgressState(false);
        return true;
    }
    return false;
}


/**
 * @brief NotificationManager::setNotificationLoading
 * @param ID
 * @param on
 * @return
 */
bool NotificationManager::setNotificationLoading(int ID, bool on)
{
    auto notification = manager()->notifications.value(ID,0);
    if(notification){
        notification->setInProgressState(on);
    }
    return notification != 0;
}


/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * Send a signal to update the warning/error count in the notification toolbar.
 * @param ID
 */
void NotificationManager::deleteNotification(int ID)
{
    if(notifications.contains(ID)){
        auto notification = notifications.take(ID);
        auto notifications_count = notifications.count();
        
        if(notification == latest_notification){
            if(notifications_count > 0){
                //Get the last notification
                latest_notification = notifications.last();
            }else{
                //Unset
                latest_notification = 0;
            }
        }

        // delete the item from the notification dialog
        emit notificationDeleted(ID);
        
        delete notification;
    }
}

NotificationObject* NotificationManager::getNotification(int id){
    return notifications.value(id, 0);
}

NotificationObject* NotificationManager::getLatestNotification(){
    return latest_notification;
}

int NotificationManager::addNotification(QString description, QString iconPath, QString iconName, int entityID, Notification::Severity severity, Notification::Type type, Notification::Category category, bool toast)
{
    auto notification = new NotificationObject();
    notification->setDescription(description);
    notification->setIcon(iconPath, iconName);
    notification->setSeverity(severity);
    notification->setEntityID(entityID);
    notification->setType(type);
    notification->setCategory(category);
    notification->setToastable(toast);

    auto notification_id = notification->getID();
    notifications[notification_id] = notification;
    connect(notification, &NotificationObject::notificationChanged, this, &NotificationManager::NotificationUpdated);

    emit notificationAdded(notification);
    NotificationUpdated(notification);
    return notification_id;
}

void NotificationManager::NotificationUpdated(NotificationObject* notification){
    if(notification){
        auto notification_id = notification->getID();
        latest_notification = notification;
        emit notificationUpdated(notification_id);
    
        if (notification->getToastable()) {
            emit toastNotification(notification);
        }
    }
}