#ifndef NOTIFICATIONOBJECT_H
#define NOTIFICATIONOBJECT_H

#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QEnableSharedFromThis>

#include "notificationenumerations.h"

class NotificationManager;
class NotificationObject : public QObject, public QEnableSharedFromThis<NotificationObject>
{
    friend class NotificationManager;
    Q_OBJECT

protected:
    NotificationObject();
    
public:
    //Setters
    void setTitle(const QString& title);
    void setDescription(const QString& description);
    void setIcon(const QString& path, const QString& name);

    void setSeverity(Notification::Severity severity);
    void setType(Notification::Type type);
    void setCategory(Notification::Category category);
    void setEntityID(int entity_id);
    void setToastEnabled(bool enable);

    bool isToastEnabled() const;
    int getID() const;
    int getEntityID() const;
    QString getTitle() const;
    QString getDescription() const;
    QPair<QString, QString> getIcon() const;
    QDateTime getModifiedTime() const;

    Notification::Type getType() const;
    Notification::Category getCategory() const;
    Notification::Severity getSeverity() const;
    
signals:
    void titleChanged();
    void descriptionChanged();
    void iconChanged();
    void progressStateChanged();
    void severityChanged();
    void typeChanged();
    void categoryChanged();
    void timeChanged();
    void notificationChanged(QSharedPointer<NotificationObject>);
    
private:
    void updateModifiedTime();
    static int id_counter_;

    int id_ = -1;
    int entity_id_ = -1;
    bool toast_enabled_ = true;

    QString title_;
    QString description_;
    QPair<QString, QString> icon_;

    Notification::Type type_ = Notification::Type::MODEL;
    Notification::Category category_ = Notification::Category::NONE;
    Notification::Severity severity_ = Notification::Severity::INFO;

    QDateTime modified_time_;
};

#endif // NOTIFICATIONOBJECT_H
