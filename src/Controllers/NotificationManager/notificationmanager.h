#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "../../enumerations.h"
#include "notificationEnumerations.h"
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
                                   NOTIFICATION_SEVERITY s = NOTIFICATION_SEVERITY::INFO,
                                   NOTIFICATION_TYPE t = NOTIFICATION_TYPE::MODEL,
                                   NOTIFICATION_CATEGORY c = NOTIFICATION_CATEGORY::NONE);

    static int displayLoadingNotification(QString description,
                                          QString iconPath = "",
                                          QString iconName = "",
                                          int entityID = -1,
                                          NOTIFICATION_SEVERITY s = NOTIFICATION_SEVERITY::INFO,
                                          NOTIFICATION_TYPE t = NOTIFICATION_TYPE::MODEL,
                                          NOTIFICATION_CATEGORY c = NOTIFICATION_CATEGORY::NONE);

    static bool updateNotification(int ID, QString description, QString iconPath, QString iconName, NOTIFICATION_SEVERITY severity);
    static bool setNotificationLoading(int ID, bool on);

    QList<NotificationObject*> getNotificationItems();
    QList<int> getNotificationsOfType(NOTIFICATION_TYPE type);
    QList<int> getNotificationsOfSeverity(NOTIFICATION_SEVERITY severity);
    QList<int> getNotificationsOfCategory(NOTIFICATION_CATEGORY category);

    NotificationObject* getNotificationItem(int id);
    NotificationObject* getLastNotificationItem();

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

    void selectionChanged(int selectedID);

public slots:
    void popupLatestNotification();
    void centerPopup();
    void deleteNotification(int ID);
    void setLastNotificationItem(int ID);

    void activeSelectionChanged(ViewItem* item, bool isActive);

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


    NotificationPopup* notification_popup = 0;

    ViewController* viewController;

};

#endif // NOTIFICATIONMANAGER_H
