#ifndef NOTIFICATIONOBJECT_H
#define NOTIFICATIONOBJECT_H

#include <QObject>

#include "../../Controllers/NotificationManager/notificationmanager.h"

class NotificationObject : public QObject
{
    friend class NotificationManager;
    Q_OBJECT

protected:
    explicit NotificationObject(QString title,
                                QString description,
                                QString iconPath,
                                QString iconName,
                                int entityID,
                                NOTIFICATION_SEVERITY severity = NOTIFICATION_SEVERITY::INFO,
                                NOTIFICATION_TYPE type = NOTIFICATION_TYPE::MODEL,
                                NOTIFICATION_CATEGORY category = NOTIFICATION_CATEGORY::NOCATEGORY,
                                QObject *parent = 0);
    ~NotificationObject();

    void setTitle(QString title);
    void setDescription(QString description);
    void setIcon(QString path, QString name);
    void setSeverity(NOTIFICATION_SEVERITY severity);

public:
    int ID();
    int entityID();

    QString title();
    QString description();
    QString iconPath();
    QString iconName();

    NOTIFICATION_TYPE type();
    NOTIFICATION_CATEGORY category();
    NOTIFICATION_SEVERITY severity();
 
signals:
    void titleChanged(QString title);
    void descriptionChanged(QString description);
    void iconChanged(QString path, QString name);

private:
    static int _NotificationID;
    int _ID;
    int _entityID;

    QString _title;
    QString _description;
    QString _iconPath;
    QString _iconName;

    NOTIFICATION_TYPE _type;
    NOTIFICATION_CATEGORY _category;
    NOTIFICATION_SEVERITY _severity;

};

#endif // NOTIFICATIONOBJECT_H
