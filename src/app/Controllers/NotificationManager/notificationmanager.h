#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "notificationenumerations.h"
#include "../ActionController/actioncontroller.h"
#include "notificationobject.h"

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

private:
    NotificationManager(ViewController* controller);
    ~NotificationManager();

protected:
    static bool construct_singleton(ViewController* controller);
    static void destruct_singleton();

public:
    static NotificationManager* manager();

    NotificationDialog* getPanel();
    NotificationToolbar* getToolbar();
    NotificationPopup* getToast();
    QSharedPointer<NotificationObject> AddNotification(QString title, QString icon_path, QString icon_name, Notification::Severity severity, Notification::Type type, Notification::Category category, bool toast = true, int entity_id = -1, bool defer_update=false);

    QList<QSharedPointer<NotificationObject>> getNotifications();
    
    QList<QSharedPointer<NotificationObject> > getNotificationsOfType(Notification::Type type);
    QList<QSharedPointer<NotificationObject> > getNotificationsOfSeverity(Notification::Severity severity);
    QList<QSharedPointer<NotificationObject> > getNotificationsOfCategory(Notification::Category category);

    QSharedPointer<NotificationObject> getNotification(int id);
    QSharedPointer<NotificationObject> getLatestNotification();

    void ShowNotificationPanel(Notification::Severity severity);

signals:
    void toastNotification(QSharedPointer<NotificationObject> notification);
    void notificationUpdated(QSharedPointer<NotificationObject> notification);
    //void notificationDeleted(int ID);
    void notificationDeleted(QSharedPointer<NotificationObject> notification);
    void notificationAdded(QSharedPointer<NotificationObject> notification);
    
    void notificationsSeen();
    void showNotificationPanel();
public slots:
    void hideToast();
    void toastLatestNotification();
    void deleteNotification(int ID);
private slots:
    void centerPopup();
    void displayToastNotification(QSharedPointer<NotificationObject> notification);
private:
    void NotificationUpdated(QSharedPointer<NotificationObject> notification);

    static NotificationManager* managerSingleton;
    
    
    QMap<int, QSharedPointer<NotificationObject>> notifications;
    
    QSharedPointer<NotificationObject> latest_notification;
    NotificationDialog* notification_panel = 0;
    NotificationPopup* notification_popup = 0;
    NotificationToolbar* notification_toolbar = 0;
    ViewController* viewController = 0;
};

#endif // NOTIFICATIONMANAGER_H
