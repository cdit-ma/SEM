#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "enumerations.h"
//#include "../ViewController/viewcontroller.h"
#include "../ActionController/actioncontroller.h"

#include <QObject>
#include <QTime>

class ViewController;
class NotificationObject;

enum NOTIFICATION_TYPE2{NT_APPLICATION, NT_MODEL};
enum NOTIFICATION_CATEGORY{NC_NOCATEGORY, NC_FILE, NC_JENKINS, NC_DEPLOYMENT, NC_VALIDATION};
enum NOTIFICATION_SEVERITY{NS_INFO, NS_WARNING, NS_ERROR};
enum NOTIFICATION_FILTER{NF_NOFILTER, NF_SEVERITY, NF_TYPE, NF_CATEGORY};

enum BACKGROUND_PROCESS{BP_UNKNOWN, BP_VALIDATION, BP_IMPORT_JENKINS};

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    static NotificationManager* manager();
    static QTime* projectTime();

    static QList<NotificationObject*> getNotificationItems();
    static QList<BACKGROUND_PROCESS> getBackgroundProcesses();
    static QList<NOTIFICATION_FILTER> getNotificationFilters();
    static QList<NOTIFICATION_TYPE2> getNotificationTypes();
    static QList<NOTIFICATION_CATEGORY> getNotificationCategories();
    static QList<NOTIFICATION_SEVERITY> getNotificationSeverities();
    static QString getTypeString(NOTIFICATION_TYPE2 type);
    static QString getCategoryString(NOTIFICATION_CATEGORY category);
    static QString getSeverityString(NOTIFICATION_SEVERITY severity);
    static QColor getSeverityColor(NOTIFICATION_SEVERITY severity);
    static QString getSeverityColorStr(NOTIFICATION_SEVERITY severity);
    static QPair<QString, QString> getSeverityIcon(NOTIFICATION_SEVERITY severity);

    void resetManager();
    void tearDown();

    void displayNotification(QString description,
                             QString iconPath = "",
                             QString iconName = "",
                             int entityID = -1,
                             NOTIFICATION_SEVERITY s = NS_INFO,
                             NOTIFICATION_TYPE2 t = NT_MODEL,
                             NOTIFICATION_CATEGORY c = NC_NOCATEGORY);

signals:
    void notificationAlert();
    void notificationSeen();

    void notificationAdded(QString iconPath, QString iconName, QString description);
    void notificationItemAdded(NotificationObject* obj);
    void notificationDeleted(int ID);

    void lastNotificationDeleted();
    void req_lastNotificationID();

    void clearNotifications(NOTIFICATION_FILTER filter = NF_NOFILTER, int filterVal = -1);

    void backgroundProcess(bool inProgress, BACKGROUND_PROCESS process = BP_UNKNOWN);

    void showNotificationDialog(bool show = true);

    void updateNotificationToolbarSize();

public slots:
    void deleteNotification(int ID);

    void setLastNotificationItem(int ID);
    void showLastNotification();

    void modelValidated(QStringList report);

private:
    void addNotification(QString description,
                         QString iconPath,
                         QString iconName,
                         int entityID,
                         NOTIFICATION_SEVERITY s,
                         NOTIFICATION_TYPE2 t,
                         NOTIFICATION_CATEGORY c,
                         bool toast = true);

    static NotificationManager* managerSingleton;
    static QTime* projectRunTime;

    static NotificationObject* lastNotificationObject;
    static QHash<int, NotificationObject*> notificationObjects;

};

#endif // NOTIFICATIONMANAGER_H
