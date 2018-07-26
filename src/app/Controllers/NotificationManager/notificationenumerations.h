#ifndef NOTIFICATIONENUMERATIONS_H
#define NOTIFICATIONENUMERATIONS_H

#include <QSet>
#include <QColor>
#include <QMetaType>

namespace Notification{
    enum class Context{NOT_SELECTED, SELECTED};
    enum class Type{MODEL, APPLICATION};
    enum class Category{NONE, FILE, JENKINS, VALIDATION};
#undef ERROR
    enum class Severity{NONE, RUNNING, INFO, WARNING, ERROR, SUCCESS};

    //Enum getters
    extern QSet<Notification::Context> getContexts();
    extern QSet<Notification::Type> getTypes();
    extern QSet<Notification::Category> getCategories();
    extern QSet<Notification::Severity> getSeverities();

    extern const QList<Notification::Severity>& getSortedSeverities();

    //String Getters
    extern QString getContextString(Notification::Context context);
    extern QString getTypeString(Notification::Type type);
    extern QString getCategoryString(Notification::Category category);
    extern QString getSeverityString(Notification::Severity severity);




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
#endif // NOTIFICATIONENUMERATIONS_H
