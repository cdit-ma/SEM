#ifndef NOTIFICATIONOBJECT_H
#define NOTIFICATIONOBJECT_H

#include <QObject>
#include <QDateTime>

#include "../../Controllers/NotificationManager/notificationmanager.h"

class NotificationObject : public QObject
{
    friend class NotificationManager;
    Q_OBJECT

protected:
    explicit NotificationObject(QString title = "",
                                QString description = "",
                                QString iconPath = "",
                                QString iconName = "",
                                int entityID = -1,
                                NOTIFICATION_SEVERITY severity = NOTIFICATION_SEVERITY::INFO,
                                NOTIFICATION_TYPE type = NOTIFICATION_TYPE::MODEL,
                                NOTIFICATION_CATEGORY category = NOTIFICATION_CATEGORY::NOCATEGORY,
                                QObject *parent = 0);
    ~NotificationObject();

    void setTitle(QString title);
    void setDescription(QString description);
    void setIcon(QString path, QString name);
    void setSeverity(NOTIFICATION_SEVERITY severity);
    void setDateTime(QDateTime dateTime);
    void setLoading(bool on);

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

    QDateTime creationDateTime();
 
signals:
    // These signals should only be triggered by the setter functions above to keep the object/item state in sync
    void titleChanged(QString title);
    void descriptionChanged(QString description);
    void iconChanged(QString path, QString name);
    void timestampChanged(QTime time);
    void loading(bool on);

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

    QDateTime _creationDateTime;

};

#endif // NOTIFICATIONOBJECT_H
