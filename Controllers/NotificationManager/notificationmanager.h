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
    explicit NotificationManager(ViewController* vc, QObject *parent = 0);

signals:
    void notificationAdded(QString iconPath, QString iconName, QString description);

public slots:
    void notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int ID);
    void notificationsSeen();
    void deleteNotification(int ID);

private:
    struct Notification {
        NOTIFICATION_TYPE type;
        QString title;
        QString description;
        QString iconPath;
        QString iconName;
        int ID;
    };

    ViewController* viewController;
    RootAction* toggleNotificationsDialog;
    NotificationDialog* notificationDialog;

    QHash<int, Notification> notifications;

};

#endif // NOTIFICATIONMANAGER_H
