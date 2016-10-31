#include "notificationitem.h"


int NotificationItem::_NotificationID  = 0;


/**
 * @brief NotificationItem::NotificationItem
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
NotificationItem::NotificationItem(QString title, QString description, QString iconPath, QString iconName, int entityID, NotificationManager::NOTIFICATION_TYPE2 type, NotificationManager::NOTIFICATION_CATEGORY category, NotificationManager::NOTIFICATION_SEVERITY severity, QObject *parent)
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
}


/**
 * @brief NotificationItem::ID
 * @return
 */
int NotificationItem::ID()
{
    return _ID;
}


/**
 * @brief NotificationItem::entityID
 * @return
 */
int NotificationItem::entityID()
{
    return _entityID;
}


/**
 * @brief NotificationItem::title
 * @return
 */
QString NotificationItem::title()
{
    return _title;
}


/**
 * @brief NotificationItem::description
 * @return
 */
QString NotificationItem::description()
{
    return _description;
}


/**
 * @brief NotificationItem::iconPath
 * @return
 */
QString NotificationItem::iconPath()
{
    return _iconPath;
}


/**
 * @brief NotificationItem::iconName
 * @return
 */
QString NotificationItem::iconName()
{
    return _iconName;
}


/**
 * @brief NotificationItem::type
 * @return
 */
NotificationManager::NOTIFICATION_TYPE2 NotificationItem::type()
{
    return _type;
}


/**
 * @brief NotificationItem::category
 * @return
 */
NotificationManager::NOTIFICATION_CATEGORY NotificationItem::category()
{
    return _category;
}


/**
 * @brief NotificationItem::severity
 * @return
 */
NotificationManager::NOTIFICATION_SEVERITY NotificationItem::severity()
{
    return _severity;
}

