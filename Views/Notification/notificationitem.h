#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include "../../Controllers/NotificationManager/notificationmanager.h"

#include <QObject>

//class NotificationManager;
class NotificationItem : public QObject
{
    Q_OBJECT
public:
    explicit NotificationItem( QString title,
                               QString description,
                               QString iconPath,
                               QString iconName,
                               int entityID,
                               NotificationManager::NOTIFICATION_TYPE2 type = NotificationManager::NT_MODEL,
                               NotificationManager::NOTIFICATION_CATEGORY category = NotificationManager::NC_NONE,
                               NotificationManager::NOTIFICATION_SEVERITY severity = NotificationManager::NS_INFO,
                               QObject *parent = 0);

    int ID();
    int entityID();

    QString title();
    QString description();
    QString iconPath();
    QString iconName();

    NotificationManager::NOTIFICATION_TYPE2 type();
    NotificationManager::NOTIFICATION_CATEGORY category();
    NotificationManager::NOTIFICATION_SEVERITY severity();

signals:

public slots:

private:
    static int _NotificationID;
    int _ID;
    int _entityID;

    QString _title;
    QString _description;
    QString _iconPath;
    QString _iconName;

    NotificationManager::NOTIFICATION_TYPE2 _type;
    NotificationManager::NOTIFICATION_CATEGORY _category;
    NotificationManager::NOTIFICATION_SEVERITY _severity;

};

#endif // NOTIFICATIONITEM_H
