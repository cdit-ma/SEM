#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include <QObject>

#include "../../Controllers/NotificationManager/notificationmanager.h"

class NotificationObject : public QObject
{
    Q_OBJECT
public:
    explicit NotificationObject(QString title,
                                QString description,
                                QString iconPath,
                                QString iconName,
                                int entityID,
                                NOTIFICATION_SEVERITY severity = NS_INFO,
                                NOTIFICATION_TYPE2 type = NT_MODEL,
                                NOTIFICATION_CATEGORY category = NC_NOCATEGORY,
                                QObject *parent = 0);

    int ID();
    int entityID();

    QString title();
    QString description();
    QString iconPath();
    QString iconName();

    NOTIFICATION_TYPE2 type();
    NOTIFICATION_CATEGORY category();
    NOTIFICATION_SEVERITY severity();

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

    NOTIFICATION_TYPE2 _type;
    NOTIFICATION_CATEGORY _category;
    NOTIFICATION_SEVERITY _severity;

};

#endif // NOTIFICATIONITEM_H
