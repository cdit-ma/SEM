#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "../../enumerations.h"
#include "notificationEnumerations.h"
#include "../ActionController/actioncontroller.h"

#include <QObject>
#include <QTime>

class ViewController;
class NotificationDialog;
class NotificationToolbar;
class NotificationObject;
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
    //static PopupWidget* constructPopup();

    static int displayNotification(QString description,
                                   QString iconPath = "",
                                   QString iconName = "",
                                   int entityID = -1,
                                   NOTIFICATION_SEVERITY s = NOTIFICATION_SEVERITY::INFO,
                                   NOTIFICATION_TYPE t = NOTIFICATION_TYPE::MODEL,
                                   NOTIFICATION_CATEGORY c = NOTIFICATION_CATEGORY::NOCATEGORY);

    static int displayLoadingNotification(QString description,
                                          QString iconPath = "",
                                          QString iconName = "",
                                          int entityID = -1,
                                          NOTIFICATION_SEVERITY s = NOTIFICATION_SEVERITY::INFO,
                                          NOTIFICATION_TYPE t = NOTIFICATION_TYPE::MODEL,
                                          NOTIFICATION_CATEGORY c = NOTIFICATION_CATEGORY::NOCATEGORY);

    static bool updateNotification(int ID, QString description, QString iconPath, QString iconName, NOTIFICATION_SEVERITY severity);
    static bool setNotificationLoading(int ID, bool on);

    QList<NotificationObject*> getNotificationItems();
    QList<int> getNotificationsOfType(NOTIFICATION_TYPE type);
    QList<int> getNotificationsOfSeverity(NOTIFICATION_SEVERITY severity);
    QList<int> getNotificationsOfCategory(NOTIFICATION_CATEGORY category);

    void clearModelNotifications();

signals:
    void notificationAlert();
    void notificationSeen();

    void notificationAdded(QString iconPath, QString iconName, QString description);
    void notificationItemAdded(NotificationObject* obj);
    void notificationDeleted(int ID);

    void updateSeverityCount(NOTIFICATION_SEVERITY severity, int count);
    void updateNotificationToolbarSize();

    void lastNotificationDeleted();
    void req_lastNotificationID();

    void showNotificationPanel(bool show = true);

    void clearNotifications(NOTIFICATION_FILTER filter = NOTIFICATION_FILTER::NOFILTER, int filterVal = -1);

public slots:
    void deleteNotification(int ID);

    void setLastNotificationItem(int ID);
    void showLastNotification();

private:
    int addNotification(QString description,
                         QString iconPath,
                         QString iconName,
                         int entityID,
                         NOTIFICATION_SEVERITY s,
                         NOTIFICATION_TYPE t,
                         NOTIFICATION_CATEGORY c,
                         bool toast = true);

    void updateSeverityCountHash(NOTIFICATION_SEVERITY severity, bool increment);

    QHash<NOTIFICATION_SEVERITY, int> severityCount;

    static NotificationManager* managerSingleton;
    static NotificationObject* lastNotificationObject;
    static QHash<int, NotificationObject*> notificationObjects;

    ViewController* viewController;

};

#endif // NOTIFICATIONMANAGER_H
