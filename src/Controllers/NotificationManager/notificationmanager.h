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

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    static NotificationManager* manager();
    static QTime* projectTime();

    static void resetManager();
    static void tearDown();

    static QList<NotificationObject*> getNotificationItems();
    static QList<NOTIFICATION_TYPE2> getNotificationTypes();
    static QList<NOTIFICATION_CATEGORY> getNotificationCategories();
    static QList<NOTIFICATION_SEVERITY> getNotificationSeverities();
    static QString getTypeString(NOTIFICATION_TYPE2 type);
    static QString getCategoryString(NOTIFICATION_CATEGORY category);
    static QString getSeverityString(NOTIFICATION_SEVERITY severity);
    static QString getSeverityColorStr(NOTIFICATION_SEVERITY severity);

    void displayNotification(QString description,
                             QString iconPath,
                             QString iconName,
                             int entityID,
                             NOTIFICATION_SEVERITY s = NS_INFO,
                             NOTIFICATION_TYPE2 t = NT_MODEL,
                             NOTIFICATION_CATEGORY c = NC_NOCATEGORY);

signals:
    void notificationAlert();
    void notificationSeen();

    void notificationAdded(QString iconPath, QString iconName, QString description);
    void notificationItemAdded(NotificationObject* item);

    void lastNotificationDeleted();
    void req_lastNotificationID();

public slots:
    void notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int entityID);
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

    static NotificationObject* lastNotificationItem;
    static QHash<int, NotificationObject*> notificationItems;

};

#endif // NOTIFICATIONMANAGER_H
