#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "../../enumerations.h"
#include "notificationenumerations.h"
#include "../ActionController/actioncontroller.h"

#include <QObject>

class ViewController;
class NotificationDialog;
class NotificationToolbar;
class NotificationObject;
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

    static int displayNotification(QString description,
                                   QString iconPath = "",
                                   QString iconName = "",
                                   int entityID = -1,
                                   Notification::Severity s = Notification::Severity::INFO,
                                   Notification::Type t = Notification::Type::MODEL,
                                   Notification::Category c = Notification::Category::NONE);

    static int displayLoadingNotification(QString description,
                                          QString iconPath = "",
                                          QString iconName = "",
                                          int entityID = -1,
                                          Notification::Severity s = Notification::Severity::INFO,
                                          Notification::Type t = Notification::Type::MODEL,
                                          Notification::Category c = Notification::Category::NONE);

    static bool updateNotification(int ID, QString description, QString iconPath, QString iconName, Notification::Severity severity);
    static bool setNotificationLoading(int ID, bool on);

    QList<NotificationObject*> getNotifications();
    
    QList<NotificationObject*> getNotificationsOfType(Notification::Type type);
    QList<NotificationObject*> getNotificationsOfSeverity(Notification::Severity severity);
    QList<NotificationObject*> getNotificationsOfCategory(Notification::Category category);

    NotificationObject* getNotification(int id);
    NotificationObject* getLatestNotification();

signals:
    void toastNotification(NotificationObject* notification);
    void notificationUpdated(int ID);
    void notificationDeleted(int ID);
    void notificationAdded(NotificationObject* notification);
    
    void notificationsSeen();
    void showNotificationPanel();

public slots:
    void hideToast();
    void toastLatestNotification();
    void deleteNotification(int ID);
private slots:
    void centerPopup();
private:
    void NotificationUpdated(NotificationObject* notification);

    int addNotification(QString description,
                        QString iconPath,
                        QString iconName,
                        int entityID,
                        Notification::Severity s,
                        Notification::Type t,
                        Notification::Category c,
                        bool toast = true);


    static NotificationManager* managerSingleton;
    
    
    QMap<int, NotificationObject*> notifications;
    
    NotificationObject* latest_notification = 0;
    NotificationDialog* notification_panel = 0;
    NotificationPopup* notification_popup = 0;
    NotificationToolbar* notification_toolbar = 0;
    ViewController* viewController = 0;
};

#endif // NOTIFICATIONMANAGER_H
