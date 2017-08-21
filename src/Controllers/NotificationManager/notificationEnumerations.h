#ifndef NOTIFICATIONENUMERATIONS_H
#define NOTIFICATIONENUMERATIONS_H

#include <QList>
#include <QColor>
#include <QMetaType>


enum class NOTIFICATION_CONTEXT{NOT_SELECTED, SELECTED};
enum class NOTIFICATION_TYPE{APPLICATION, MODEL};
enum class NOTIFICATION_CATEGORY{NONE, FILE, JENKINS, VALIDATION};
enum class NOTIFICATION_SEVERITY{INFO, WARNING, ERROR};
enum class NOTIFICATION_FILTER{NOFILTER, SEVERITY, TYPE, CATEGORY};

extern QList<NOTIFICATION_CONTEXT> getNotificationContexts();
extern QList<NOTIFICATION_FILTER> getNotificationFilters();
extern QList<NOTIFICATION_TYPE> getNotificationTypes();
extern QList<NOTIFICATION_CATEGORY> getNotificationCategories();
extern QList<NOTIFICATION_SEVERITY> getNotificationSeverities();

extern QString getTypeString(NOTIFICATION_TYPE type);
extern QString getSeverityString(NOTIFICATION_SEVERITY severity);
extern QString getCategoryString(NOTIFICATION_CATEGORY category);
extern QString getContextString(NOTIFICATION_CONTEXT category);

extern QString getSeverityIcon(NOTIFICATION_SEVERITY severity);
extern QString getCategoryIcon(NOTIFICATION_CATEGORY category);
extern QString getContextIcon(NOTIFICATION_CONTEXT context);
extern QString getTypeIcon(NOTIFICATION_TYPE type);

extern QColor getSeverityColor(NOTIFICATION_SEVERITY severity);
extern QString getSeverityColorStr(NOTIFICATION_SEVERITY severity);


inline uint qHash(NOTIFICATION_FILTER key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(NOTIFICATION_FILTER)

inline uint qHash(NOTIFICATION_SEVERITY key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(NOTIFICATION_SEVERITY)

inline uint qHash(NOTIFICATION_CATEGORY key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(NOTIFICATION_CATEGORY)

inline uint qHash(NOTIFICATION_CONTEXT key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(NOTIFICATION_CONTEXT)

inline uint qHash(NOTIFICATION_TYPE key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(NOTIFICATION_TYPE)



#endif // NOTIFICATIONENUMERATIONS_H
