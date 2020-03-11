#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "notificationenumerations.h"
#include "notificationobject.h"
#include "../ActionController/actioncontroller.h"

#include <QObject>

class ViewController;
class NotificationDialog;
class NotificationToolbar;
class NotificationPopup;
class PopupWidget;

class NotificationManager : public QObject
{
Q_OBJECT
    friend class ViewController;

protected:
    static bool construct_singleton(ViewController* controller);
    
    // TODO: This isn't being used; should it be called to delete the singleton at some point?
    static void destruct_singleton();

public:
    static NotificationManager* manager();
    
    NotificationDialog* getPanel();
    NotificationToolbar* getToolbar();
    NotificationPopup* getToast();
    QSharedPointer<NotificationObject> AddNotification(const QString& title, const QString& icon_path, const QString& icon_name, Notification::Severity severity, Notification::Type type, Notification::Category category, bool toast = true, int entity_id = -1, bool defer_update=false);
    
    QList<QSharedPointer<NotificationObject>> getNotifications();
    
    QList<QSharedPointer<NotificationObject> > getNotificationsOfType(Notification::Type type);
    QList<QSharedPointer<NotificationObject> > getNotificationsOfCategory(Notification::Category category);
    
    QSharedPointer<NotificationObject> getLatestNotification();
    
    void ShowNotificationPanel(Notification::Severity severity);

signals:
    void toastNotification(QSharedPointer<NotificationObject> notification);
    void notificationUpdated(QSharedPointer<NotificationObject> notification);
    void notificationDeleted(QSharedPointer<NotificationObject> notification);
    void notificationAdded(QSharedPointer<NotificationObject> notification);
    
    void notificationsSeen();
    void showNotificationPanel();

public slots:
    void hideToast();
    void toastLatestNotification();
    void deleteNotification(int ID);

private slots:
    void displayToastNotification(QSharedPointer<NotificationObject> notification);

private:
    explicit NotificationManager(ViewController* controller);
    
    // TODO: Memory management???
    ~NotificationManager() final = default;
    
    void NotificationUpdated(QSharedPointer<NotificationObject> notification);
    
    static NotificationManager* managerSingleton;
    
    QMap<int, QSharedPointer<NotificationObject>> notifications;
    QSharedPointer<NotificationObject> latest_notification;
    
    NotificationDialog* notification_panel = nullptr;
    NotificationPopup* notification_popup = nullptr;
    NotificationToolbar* notification_toolbar = nullptr;
    ViewController* viewController = nullptr;
};

#endif // NOTIFICATIONMANAGER_H
