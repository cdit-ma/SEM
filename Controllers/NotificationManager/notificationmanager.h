#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "enumerations.h"
#include "../ViewController/viewcontroller.h"
#include "../ActionController/actioncontroller.h"
#include "../../Widgets/Dialogs/notificationdialog.h"

#include <QObject>


class NotificationManager : public QObject
{
    Q_OBJECT
public:
    static NotificationManager* manager();
    static void tearDown();

    explicit NotificationManager(ViewController* vc, QWidget *parent = 0);

signals:
    void notificationAdded(QString iconPath, QString iconName, QString description);
    void notificationSeen();
    void lastNotificationDeleted();

public slots:
    void notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int ID);
    void deleteNotification(int ID);
    void showLastNotification();

private:
    struct Notification {
        NOTIFICATION_TYPE type;
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
    QHash<NOTIFICATION_TYPE, QLabel*> typeCount;

};

#endif // NOTIFICATIONMANAGER_H
