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

    static NotificationDialog* constructPanel();
    static NotificationToolbar* constructToolbar();
    NotificationPopup* getNotificationPopup();

    QList<int> getSelectionIDs();


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

    QList<NotificationObject*> getNotificationItems();
    QList<int> getNotificationsOfType(Notification::Type type);
    QList<int> getNotificationsOfSeverity(Notification::Severity severity);
    QList<int> getNotificationsOfCategory(Notification::Category category);

    NotificationObject* getNotificationItem(int id);
    NotificationObject* getLastNotificationItem();

    void clearModelNotifications();

signals:
    void notificationAlert();
    void notificationSeen();

    void notificationAdded(QString iconPath, QString iconName, QString description);
    void notificationItemAdded(NotificationObject* obj);
    void notificationDeleted(int ID);

    void updateSeverityCount(Notification::Severity severity, int count);
    void updateNotificationToolbarSize();

    void lastNotificationDeleted();
    void req_lastNotificationID();

    void showNotificationPanel(bool show = true);

public slots:
    void popupLatestNotification();
    void centerPopup();
    void deleteNotification(int ID);
    void setLastNotificationItem(int ID);
private:
    int addNotification(QString description,
                        QString iconPath,
                        QString iconName,
                        int entityID,
                        Notification::Severity s,
                        Notification::Type t,
                        Notification::Category c,
                        bool toast = true);

    void updateSeverityCountHash(Notification::Severity severity, bool increment);

    QHash<Notification::Severity, int> severityCount;

    static NotificationManager* managerSingleton;
    static NotificationObject* lastNotificationObject;
    static QHash<int, NotificationObject*> notificationObjects;


    NotificationPopup* notification_popup = 0;

    ViewController* viewController;

};

#endif // NOTIFICATIONMANAGER_H
