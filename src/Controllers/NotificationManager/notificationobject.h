#ifndef NOTIFICATIONOBJECT_H
#define NOTIFICATIONOBJECT_H

#include <QObject>
#include <QDateTime>

#include "notificationenumerations.h"

class NotificationManager;
class NotificationObject : public QObject
{
    friend class NotificationManager;
    Q_OBJECT

protected:
    NotificationObject();
    ~NotificationObject();
public:
    //Setters
    void setTitle(QString title);
    void setDescription(QString description);
    void setIcon(QString path, QString name);

    void setSeverity(Notification::Severity severity);
    void setType(Notification::Type type);
    void setCategory(Notification::Category category);
    void setEntityID(int entity_id);
    void setInProgressState(bool in_progress = true);
    void setToastable(bool toast);

    bool getToastable() const;
    int getID() const;
    int getEntityID() const;
    QString getTitle() const;
    QString getDescription() const;
    QPair<QString, QString> getIcon() const;
    QDateTime getModifiedTime() const;
    bool getInProgressState() const;

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
    void notificationChanged(NotificationObject*);
private:
    void updateModifiedTime();
    static int id_counter_;

    int id_ = -1;
    int entity_id_ = -1;
    bool in_progress_state_ = false;
    bool toastable_ = true;

    QString title_;
    QString description_;
    QPair<QString, QString> icon_;

    Notification::Type type_ = Notification::Type::MODEL;
    Notification::Category category_ = Notification::Category::NONE;
    Notification::Severity severity_ = Notification::Severity::INFO;

    QDateTime modified_time_;
};

#endif // NOTIFICATIONOBJECT_H
