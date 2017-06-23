#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "../../enumerations.h"
#include "../ActionController/actioncontroller.h"

#include <QObject>
#include <QTime>

class ViewController;
class NotificationDialog;
class NotificationToolbar;
class NotificationObject;
class PopupWidget;

enum NOTIFICATION_TYPE{NT_APPLICATION, NT_MODEL};
enum NOTIFICATION_CATEGORY{NC_NOCATEGORY, NC_FILE, NC_JENKINS, NC_DEPLOYMENT, NC_VALIDATION};
enum NOTIFICATION_SEVERITY{NS_INFO, NS_WARNING, NS_ERROR};
enum NOTIFICATION_FILTER{NF_NOFILTER, NF_SEVERITY, NF_TYPE, NF_CATEGORY};

enum BACKGROUND_PROCESS{BP_UNKNOWN, BP_VALIDATION, BP_IMPORT_JENKINS, BP_RUNNING_JOB};


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

    //Helper functions for enums
    static QList<BACKGROUND_PROCESS> getBackgroundProcesses();
    static QList<NOTIFICATION_FILTER> getNotificationFilters();
    static QList<NOTIFICATION_TYPE> getNotificationTypes();
    static QList<NOTIFICATION_CATEGORY> getNotificationCategories();
    static QList<NOTIFICATION_SEVERITY> getNotificationSeverities();
    static QString getTypeString(NOTIFICATION_TYPE type);
    static QString getSeverityString(NOTIFICATION_SEVERITY severity);
    static QString getSeverityIcon(NOTIFICATION_SEVERITY severity);
    static QString getCategoryString(NOTIFICATION_CATEGORY category);
    static QString getCategoryIcon(NOTIFICATION_CATEGORY category);
    static QColor getSeverityColor(NOTIFICATION_SEVERITY severity);
    static QString getSeverityColorStr(NOTIFICATION_SEVERITY severity);

    QList<NotificationObject*> getNotificationItems();

    QList<int> getNotificationsOfType(NOTIFICATION_TYPE type);
    QList<int> getNotificationsOfSeverity(NOTIFICATION_SEVERITY severity);
    QList<int> getNotificationsOfCategory(NOTIFICATION_CATEGORY category);

    void clearModelNotifications();

public:
    static int displayNotification(QString description,
                             QString iconPath = "",
                             QString iconName = "",
                             int entityID = -1,
                             NOTIFICATION_SEVERITY s = NS_INFO,
                             NOTIFICATION_TYPE t = NT_MODEL,
                             NOTIFICATION_CATEGORY c = NC_NOCATEGORY);

    static bool updateNotification(int ID, QString iconPath, QString iconName, QString description);

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

    void backgroundProcess(bool inProgress, BACKGROUND_PROCESS process = BP_UNKNOWN);

    void clearNotifications(NOTIFICATION_FILTER filter = NF_NOFILTER, int filterVal = -1);

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
