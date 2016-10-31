#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "enumerations.h"
#include "../ViewController/viewcontroller.h"
#include "../ActionController/actioncontroller.h"

#include <QObject>

class NotificationItem;
class NotificationManager : public QObject
{
    Q_OBJECT
public:
    enum NOTIFICATION_TYPE2{NT_APPLICATION, NT_MODEL};
    enum NOTIFICATION_CATEGORY{NC_NONE, NC_FILE, NC_JENKINS, NC_DEPLOYMENT};
    enum NOTIFICATION_SEVERITY{NS_INFO, NS_WARNING, NS_ERROR};

    static NotificationManager* manager();
    static void tearDown();

    static QList<NotificationItem*> getNotificationItems();

    static QList<NOTIFICATION_SEVERITY> getNotificationSeverities();
    static QString getNotificationSeverityString(NOTIFICATION_SEVERITY severity);
    static QString getNotificationSeverityColorStr(NOTIFICATION_SEVERITY severity);

signals:
    void notificationAdded(QString iconPath, QString iconName, QString description);
    void notificationItemAdded(NotificationItem* item);
    void notificationSeen();

    void lastNotificationDeleted();
    void req_lastNotificationID();

public slots:
    void notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int entityID);
    void deleteNotification(int ID);

    void setLastNotificationItem(int ID);
    void showLastNotification();

private:
    static NotificationManager* managerSingleton;
    static NotificationItem* lastNotificationItem;
    static QHash<int, NotificationItem*> notificationItems;

};

#endif // NOTIFICATIONMANAGER_H
