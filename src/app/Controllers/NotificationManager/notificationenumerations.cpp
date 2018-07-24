#include "notificationenumerations.h"
#include "../../theme.h"

class NotificationSorter{
    public:
        NotificationSorter(){
            severities = Notification::getSeverities().toList();
            type = Notification::getTypes().toList();
            categories = Notification::getCategories().toList();
            
            std::sort(severities.begin(), severities.end());
            std::sort(type.begin(), type.end());
            std::sort(categories.begin(), categories.end());
        };
        QList<Notification::Severity> severities;
        QList<Notification::Type> type;
        QList<Notification::Category> categories;
};

static NotificationSorter sorted;

QSet<Notification::Context> Notification::getContexts(){
    return {Context::NOT_SELECTED, Context::SELECTED};
}
QSet<Notification::Type> Notification::getTypes(){
    return {Type::APPLICATION, Type::MODEL};
}

QSet<Notification::Category> Notification::getCategories(){
    return {Category::NONE, Category::FILE, Category::JENKINS, Category::VALIDATION};
}

QSet<Notification::Severity> Notification::getSeverities(){
    return {Severity::NONE, Severity::RUNNING, Severity::INFO, Severity::WARNING, Severity::ERROR, Severity::SUCCESS};
}

const QList<Notification::Severity>& Notification::getSortedSeverities(){
    return sorted.severities;
}

QString Notification::getTypeString(Notification::Type type)
{
    switch (type){
    case Type::MODEL:
        return "Model";
    case Type::APPLICATION:
        return "Application";
    }
    return QString();
}

QString Notification::getSeverityString(Notification::Severity severity)
{
    switch (severity){
    case Severity::INFO:
        return "Information";
    case Severity::WARNING:
        return "Warning";
    case Severity::ERROR:
        return "Error";
    case Severity::SUCCESS:
        return "Success";
    case Severity::RUNNING:
        return "Running";
    case Severity::NONE:
        return "No Severity";
    }
    return QString();
}

QString Notification::getContextString(Notification::Context context)
{
    switch (context){
    case Context::SELECTED:
        return "Selected";
    case Context::NOT_SELECTED:
        return "Not Selected";
    }
    return QString();
}

QString Notification::getCategoryString(Notification::Category category)
{
    switch (category){
    case Category::FILE:
        return "File";
    case Category::JENKINS:
        return "Jenkins";
    case Category::VALIDATION:
        return "Validation";
    case Category::NONE:
        return "No Category";
    }
    return QString();
}
