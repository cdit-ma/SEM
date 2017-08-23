#ifndef NOTIFICATIONENUMERATIONS_H
#define NOTIFICATIONENUMERATIONS_H

#include <QList>
#include <QColor>
#include <QMetaType>

namespace Notification{
    enum class Context{NOT_SELECTED, SELECTED};
    enum class Type{APPLICATION, MODEL};
    enum class Category{NONE, FILE, JENKINS, VALIDATION};
    enum class Severity{INFO, WARNING, ERROR};

    //Enum getters
    extern QSet<Notification::Context> getContexts();
    extern QSet<Notification::Type> getTypes();
    extern QSet<Notification::Category> getCategories();
    extern QSet<Notification::Severity> getSeverities();

    //String Getters
    extern QString getContextString(Notification::Context context);
    extern QString getTypeString(Notification::Type type);
    extern QString getCategoryString(Notification::Category category);
    extern QString getSeverityString(Notification::Severity severity);

    //Color Getters
    extern QColor getSeverityColor(Notification::Severity severity);


    inline uint qHash(Notification::Context key, uint seed){
        return ::qHash(static_cast<uint>(key), seed);
    };
    
    inline uint qHash(Notification::Type key, uint seed){
        return ::qHash(static_cast<uint>(key), seed);
    };
    
    inline uint qHash(Notification::Category key, uint seed){
        return ::qHash(static_cast<uint>(key), seed);
    };
    
    inline uint qHash(Notification::Severity key, uint seed){
        return ::qHash(static_cast<uint>(key), seed);
    };
};
Q_DECLARE_METATYPE(Notification::Context);
Q_DECLARE_METATYPE(Notification::Type);
Q_DECLARE_METATYPE(Notification::Category);
Q_DECLARE_METATYPE(Notification::Severity);


enum class NOTIFICATION_CONTEXT{NOT_SELECTED, SELECTED};
enum class NOTIFICATION_TYPE{APPLICATION, MODEL};
enum class NOTIFICATION_CATEGORY{NONE, FILE, JENKINS, VALIDATION};
enum class NOTIFICATION_SEVERITY{INFO, WARNING, ERROR};

extern QList<NOTIFICATION_CONTEXT> getNotificationContexts();
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
