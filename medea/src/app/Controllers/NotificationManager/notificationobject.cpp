#include "notificationobject.h"
#include "notificationmanager.h"

int NotificationObject::id_counter_  = 0;

NotificationObject::NotificationObject()
    : QObject(nullptr)
{
    id_ = id_counter_ ++;
    updateModifiedTime();
    
    //Update the modified time any time any part of this notification gets updated
    connect(this, &NotificationObject::titleChanged, &NotificationObject::updateModifiedTime);
    connect(this, &NotificationObject::descriptionChanged, &NotificationObject::updateModifiedTime);
    connect(this, &NotificationObject::iconChanged, &NotificationObject::updateModifiedTime);
    connect(this, &NotificationObject::progressStateChanged, &NotificationObject::updateModifiedTime);
    connect(this, &NotificationObject::severityChanged, &NotificationObject::updateModifiedTime);
    connect(this, &NotificationObject::typeChanged, &NotificationObject::updateModifiedTime);
    connect(this, &NotificationObject::categoryChanged, &NotificationObject::updateModifiedTime);
}

void NotificationObject::setTitle(const QString& title)
{
    if(title_ != title){
        title_ = title;
        emit titleChanged();
    }    
}

void NotificationObject::setToastEnabled(bool enable)
{
    toast_enabled_ = enable;
}

void NotificationObject::setEntityID(int entity_id)
{
    if(entity_id_ != entity_id){
        entity_id_ = entity_id;
    }
}

void NotificationObject::setDescription(const QString& description)
{
    if(description_ != description){
        description_ = description;
        emit descriptionChanged();
    }
}

void NotificationObject::setIcon(const QString& path, const QString& name)
{
    auto icon = qMakePair(path, name);
    if(icon_ != icon){
        icon_ = icon;
        emit iconChanged();
    }
}

void NotificationObject::setSeverity(Notification::Severity severity)
{
    if(severity_ != severity){
        severity_ = severity;
        emit severityChanged();
    }
}

void NotificationObject::setType(Notification::Type type)
{
    if(type_ != type){
        type_ = type;
        emit typeChanged();
    }
}

void NotificationObject::setCategory(Notification::Category category){
    if(category_ != category){
        category_ = category;
        emit categoryChanged();
    }
}

bool NotificationObject::isToastEnabled() const
{
    return toast_enabled_;
}

int NotificationObject::getID() const
{
    return id_;
}

int NotificationObject::getEntityID() const
{
    return entity_id_;
}

QString NotificationObject::getTitle() const
{
    return title_;
}

QString NotificationObject::getDescription() const
{
    return description_;
}

QPair<QString, QString> NotificationObject::getIcon() const
{
    return icon_;
}

QDateTime NotificationObject::getModifiedTime() const
{
    return modified_time_;
}

Notification::Type NotificationObject::getType() const
{
    return type_;
}

Notification::Category NotificationObject::getCategory() const
{
    return category_;
}

Notification::Severity NotificationObject::getSeverity() const
{
    return severity_;
}

void NotificationObject::updateModifiedTime()
{
    modified_time_ = QDateTime::currentDateTime();
    emit timeChanged();
    emit notificationChanged(sharedFromThis());
}