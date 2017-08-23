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
                                Notification::Severity severity = Notification::Severity::INFO,
                                Notification::Type type = Notification::Type::MODEL,
                                Notification::Category category = Notification::Category::NONE,
                                QObject *parent = 0);
    ~NotificationObject();

    void setTitle(QString title);
    void setDescription(QString description);
    void setIcon(QString path, QString name);
    void setSeverity(Notification::Severity severity);
    void setDateTime(QDateTime dateTime);
    void setLoading(bool on);

public:
    int ID();
    int entityID();

    QString title();
    QString description();
    QString iconPath();
    QString iconName();
    QDateTime time() const;
    bool isLoading();

    Notification::Type type();
    Notification::Category category();
    Notification::Severity severity();

 
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
    bool _loading = false;
    QString _title;
    QString _description;
    QString _iconPath;
    QString _iconName;

    Notification::Type _type;
    Notification::Category _category;
    Notification::Severity _severity;

    QDateTime _creationDateTime;

};

#endif // NOTIFICATIONOBJECT_H
