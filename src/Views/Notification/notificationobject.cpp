#include "notificationobject.h"


int NotificationObject::_NotificationID  = 0;


/**
 * @brief NotificationObject::NotificationObject
 * @param title
 * @param description
 * @param iconPath
 * @param iconName
 * @param entityID
 * @param type
 * @param category
 * @param severity
 * @param parent
 */
NotificationObject::NotificationObject(QString title, QString description, QString iconPath, QString iconName, int entityID, Notification::Severity severity, Notification::Type type, Notification::Category category, QObject *parent)
    : QObject(parent)
{
    _ID = ++_NotificationID;
    _entityID = entityID;

    _title = title;
    _description = description;

    _type = type;
    _category = category;
    _severity = severity;
    _iconPath = iconPath;
    _iconName = iconName;

    _creationDateTime = QDateTime::currentDateTime();
}


/**
 * @brief NotificationObject::~NotificationObject
 */
NotificationObject::~NotificationObject() {}


/**
 * @brief NotificationObject::setTitle
 * @param title
 */
void NotificationObject::setTitle(QString title)
{
    _title = title;
    emit titleChanged(title);
}


bool NotificationObject::isLoading(){
    return _loading;
}

/**
 * @brief NotificationObject::setDescription
 * @param description
 */
void NotificationObject::setDescription(QString description)
{
    _description = description;
    emit descriptionChanged(description);
}


/**
 * @brief NotificationObject::setIcon
 * @param path
 * @param name
 */
void NotificationObject::setIcon(QString path, QString name)
{
    _iconPath = path;
    _iconName = name;
    emit iconChanged(path, name);
}



/**
 * @brief NotificationObject::setSeverity
 * @param severity
 */
void NotificationObject::setSeverity(Notification::Severity severity)
{
    _severity = severity;
    emit iconChanged(_iconPath, _iconName);
}


/**
 * @brief NotificationObject::setDateTime
 * @param dateTime
 */
void NotificationObject::setDateTime(QDateTime dateTime)
{
    _creationDateTime = dateTime;
    emit timestampChanged(dateTime.time());
}


/**
 * @brief NotificationObject::setLoading
 * @param on
 */
void NotificationObject::setLoading(bool on)
{
    _loading = on;
    emit loading(_loading);
}


/**
 * @brief NotificationObject::ID
 * @return
 */
int NotificationObject::ID()
{
    return _ID;
}


/**
 * @brief NotificationObject::entityID
 * @return
 */
int NotificationObject::entityID()
{
    return _entityID;
}


/**
 * @brief NotificationObject::title
 * @return
 */
QString NotificationObject::title()
{
    return _title;
}


/**
 * @brief NotificationObject::description
 * @return
 */
QString NotificationObject::description()
{
    return _description;
}


/**
 * @brief NotificationObject::iconPath
 * @return
 */
QString NotificationObject::iconPath()
{
    return _iconPath;
}


/**
 * @brief NotificationObject::iconName
 * @return
 */
QString NotificationObject::iconName()
{
    return _iconName;
}


/**
 * @brief NotificationObject::type
 * @return
 */
Notification::Type NotificationObject::type()
{
    return _type;
}


/**
 * @brief NotificationObject::category
 * @return
 */
Notification::Category NotificationObject::category()
{
    return _category;
}


/**
 * @brief NotificationObject::severity
 * @return
 */
Notification::Severity NotificationObject::severity()
{
    return _severity;
}


/**
 * @brief NotificationObject::creationDateTime
 * @return
 */
QDateTime NotificationObject::time() const
{
    return _creationDateTime;
}

