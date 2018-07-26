#include "notificationmanager.h"
#include "notificationobject.h"

#include "../WindowManager/windowmanager.h"
#include "../ViewController/viewcontroller.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Views/Notification/notificationdialog.h"
#include "../../Views/Notification/notificationtoolbar.h"
#include "../../Views/Notification/notificationpopup.h"
#include "../../Widgets/Dialogs/popupwidget.h"
#include <QSharedPointer>

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
    theme->setIconAlias(notification_str, Notification::getSeverityString(Notification::Severity::ERROR), icon_prefix, "octagonCriticalDark");
    theme->setIconAlias(notification_str, Notification::getSeverityString(Notification::Severity::SUCCESS), icon_prefix, "circleTickDark");
    theme->setIconAlias(notification_str, Notification::getSeverityString(Notification::Severity::RUNNING), icon_prefix, "running");

    //Setup Category Icons
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::JENKINS), icon_prefix, "jenkinsFlat");
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::FILE), icon_prefix, "file");
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::VALIDATION), icon_prefix, "shieldTick");
    theme->setIconAlias(notification_str, Notification::getCategoryString(Notification::Category::NONE), icon_prefix, "tiles");

    //Setup Type Icons
    theme->setIconAlias(notification_str, Notification::getTypeString(Notification::Type::MODEL), icon_prefix, "dotsInRectangle");
    theme->setIconAlias(notification_str, Notification::getTypeString(Notification::Type::APPLICATION), icon_prefix, "home");
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


void NotificationManager::displayToastNotification(QSharedPointer<NotificationObject> notification){
    if(!viewController->isWelcomeScreenShowing()){
        notification_popup->DisplayNotification(notification);
        auto window = WindowManager::manager()->getMainWindow();
        WindowManager::MoveWidget(notification_popup, window, Qt::AlignBottom);
        notification_popup->show();
    }
}

NotificationPopup* NotificationManager::getToast()
{
    if(!notification_popup){
        notification_popup = new NotificationPopup();
        connect(this, &NotificationManager::toastNotification, this, &NotificationManager::displayToastNotification, Qt::QueuedConnection);
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
        auto panel = getPanel();
        connect(notification_toolbar, &NotificationToolbar::showSeverity, panel, &NotificationDialog::showSeverity);
    }
    return notification_toolbar;
}

/**
 * @brief NotificationManager::getNotificationItems
 * @return
 */
QList<QSharedPointer<NotificationObject> > NotificationManager::getNotifications()
{
    return notifications.values();
}


/**
 * @brief NotificationManager::getNotificationsOfType
 * @param type
 * @return
 */
QList<QSharedPointer<NotificationObject> > NotificationManager::getNotificationsOfType(Notification::Type type)
{
    QList<QSharedPointer<NotificationObject> > list;
    for (auto notification : notifications.values()) {
        if (notification->getType() == type) {
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
QList<QSharedPointer<NotificationObject> > NotificationManager::getNotificationsOfCategory(Notification::Category category)
{
    QList<QSharedPointer<NotificationObject> > list;
    for (auto notification : notifications.values()) {
        if (notification->getCategory() == category) {
            list << notification;
        }
    }
    return list;
}

QSharedPointer<NotificationObject> NotificationManager::AddNotification(QString title, QString icon_path, QString icon_name, Notification::Severity severity, Notification::Type type, Notification::Category category, bool toast, int entity_id, bool defer_update){
    auto notification = QSharedPointer<NotificationObject>(new NotificationObject());
    notification->setTitle(title);
    notification->setIcon(icon_path, icon_name);
    notification->setSeverity(severity);
    notification->setType(type);
    notification->setCategory(category);
    notification->setEntityID(entity_id);
    notification->setToastable(toast);

    auto notification_id = notification->getID();
    notifications[notification_id] = notification;
    
    connect(notification.data(), &NotificationObject::notificationChanged, this, &NotificationManager::NotificationUpdated, Qt::QueuedConnection);

    if(!defer_update){
        emit notificationAdded(notification);
    }
    NotificationUpdated(notification);
    return notification;
}


void NotificationManager::toastLatestNotification(){
    auto popup = getToast(); 

    if(latest_notification){
        emit toastNotification(latest_notification);
    }
    
    
}




/**
 * @brief NotificationManager::deleteNotification
 * Remove notification with the provided ID from the hash and the notification dialog.
 * Send a signal to update the warning/error count in the notification toolbar.
 * @param ID
 */
void NotificationManager::deleteNotification(int ID)
{
    auto notification = notifications.value(ID);

    if(!notification.isNull()){
        bool can_delete = notification->getSeverity() != Notification::Severity::RUNNING;
        if(can_delete){
            notifications.remove(ID);
            auto notifications_count = notifications.count();
            

            if(notification == latest_notification){
                if(notifications_count > 0){
                    //Get the last notification
                    latest_notification = notifications.last();
                }else{
                    //Unset
                    latest_notification.reset();
                }
            }

            disconnect(notification.data());
            
            // delete the item from the notification dialog
            emit notificationDeleted(notification);
        }
    }
}

QSharedPointer<NotificationObject> NotificationManager::getLatestNotification(){
    return latest_notification;
}

void NotificationManager::NotificationUpdated(QSharedPointer<NotificationObject> notification){
    if(notification){
        auto notification_id = notification->getID();
        if(notifications.count(notification_id)){
            latest_notification = notification;
            emit notificationUpdated(notification);
        
            if (notification->getToastable()) {
                emit toastNotification(notification);
            }
        }
    }
}

void NotificationManager::ShowNotificationPanel(Notification::Severity severity){
    getPanel()->showSeveritySelection(severity);
    emit showNotificationPanel();
}