#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "enumerations.h"
#include "../ViewController/viewcontroller.h"
#include "../ActionController/actioncontroller.h"
#include "../../Widgets/Dialogs/notificationdialog.h"
//#include "../../Views/Notification/notificationitem.h"

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

signals:
    void notificationAdded(QString iconPath, QString iconName, QString description);
    void notificationItemAdded(NotificationItem* item);
    void notificationSeen();
    void lastNotificationDeleted();

public slots:
    void notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int entityID);
    void deleteNotification(int ID);
    void showLastNotification();

private:
    struct Notification {
        NOTIFICATION_TYPE2 type;
        QString title;
        QString description;
        QString iconPath;
        QString iconName;
        int entityID;
        int ID;
    };

    static NotificationManager* managerSingleton;
    static int _NotificationID;

    ViewController* viewController;

    NotificationDialog* notificationDialog;
    Notification lastNotification;

    QHash<int, Notification> notifications;
    QHash<NOTIFICATION_TYPE2, QLabel*> typeCount;

    QHash<int, NotificationItem*> notificationItems;
    NotificationItem* lastNotificationItem;

};

#endif // NOTIFICATIONMANAGER_H
