#include "notificationEnumerations.h"


/**
 * @brief getBackgroundProcesses
 * @return
 */
QList<BACKGROUND_PROCESS> getBackgroundProcesses()
{
    QList<BACKGROUND_PROCESS> processes;
    processes.append(BACKGROUND_PROCESS::UNKNOWN);
    processes.append(BACKGROUND_PROCESS::VALIDATION);
    processes.append(BACKGROUND_PROCESS::IMPORT_JENKINS);
    processes.append(BACKGROUND_PROCESS::RUNNING_JOB);
    return processes;
}


/**
 * @brief getNotificationFilters
 * @return
 */
QList<NOTIFICATION_FILTER> getNotificationFilters()
{
    QList<NOTIFICATION_FILTER> filters;
    filters.append(NOTIFICATION_FILTER::NOFILTER);
    filters.append(NOTIFICATION_FILTER::SEVERITY);
    filters.append(NOTIFICATION_FILTER::TYPE);
    filters.append(NOTIFICATION_FILTER::CATEGORY);
    return filters;
}


/**
 * @brief getNotificationTypes
 * @return
 */
QList<NOTIFICATION_TYPE> getNotificationTypes()
{
    QList<NOTIFICATION_TYPE> types;
    types.append(NOTIFICATION_TYPE::MODEL);
    types.append(NOTIFICATION_TYPE::APPLICATION);
    return types;
}


/**
 * @brief getNotificationCategories
 * @return
 */
QList<NOTIFICATION_CATEGORY> getNotificationCategories()
{
    QList<NOTIFICATION_CATEGORY> categories;
    categories.append(NOTIFICATION_CATEGORY::NOCATEGORY);
    categories.append(NOTIFICATION_CATEGORY::DEPLOYMENT);
    categories.append(NOTIFICATION_CATEGORY::FILE);
    categories.append(NOTIFICATION_CATEGORY::JENKINS);
    categories.append(NOTIFICATION_CATEGORY::VALIDATION);
    return categories;
}


/**
 * @brief getNotificationSeverities
 * @return
 */
QList<NOTIFICATION_SEVERITY> getNotificationSeverities()
{
    QList<NOTIFICATION_SEVERITY> severities;
    severities.append(NOTIFICATION_SEVERITY::INFO);
    severities.append(NOTIFICATION_SEVERITY::WARNING);
    severities.append(NOTIFICATION_SEVERITY::ERROR);
    return severities;
}


/**
 * @brief getTypeString
 * @param type
 * @return
 */
QString getTypeString(NOTIFICATION_TYPE type)
{
    switch (type) {
    case NOTIFICATION_TYPE::MODEL:
        return "Model";
    case NOTIFICATION_TYPE::APPLICATION:
        return "Application";
    default:
        return "Unknown Type";
    }
}


/**
 * @brief getSeverityString
 * @param severity
 * @return
 */
QString getSeverityString(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NOTIFICATION_SEVERITY::INFO:
        return "Information";
    case NOTIFICATION_SEVERITY::WARNING:
        return "Warning";
    case NOTIFICATION_SEVERITY::ERROR:
        return "Error";
    default:
        return "Unknown Severity";
    }
}


/**
 * @brief getCategoryString
 * @param category
 * @return
 */
QString getCategoryString(NOTIFICATION_CATEGORY category)
{
    switch (category) {
    case NOTIFICATION_CATEGORY::DEPLOYMENT:
        return "Deployment";
    case NOTIFICATION_CATEGORY::FILE:
        return "File";
    case NOTIFICATION_CATEGORY::JENKINS:
        return "Jenkins";
    case NOTIFICATION_CATEGORY::VALIDATION:
        return "Validation";
    default:
        return "No Category";
    }
}


/**
 * @brief getSeverityIcon
 * @param severity
 * @return
 */
QString getSeverityIcon(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NOTIFICATION_SEVERITY::INFO:
        return "circleInfoDark";
    case NOTIFICATION_SEVERITY::WARNING:
        return "triangleCritical";
    case NOTIFICATION_SEVERITY::ERROR:
        return "circleCrossDark";
    default:
        return "circleQuestion";
    }
}


/**
 * @brief getCategoryIcon
 * @param category
 * @return
 */
QString getCategoryIcon(NOTIFICATION_CATEGORY category)
{
    switch (category) {
    case NOTIFICATION_CATEGORY::DEPLOYMENT:
        return "screen";
    case NOTIFICATION_CATEGORY::JENKINS:
        return "jenkins";
    case NOTIFICATION_CATEGORY::FILE:
        return "file";
    case NOTIFICATION_CATEGORY::VALIDATION:
        return "shieldTick";
    default:
        return "tiles";
    }
}


/**
 * @brief getSeverityColor
 * @param severity
 * @return
 */
QColor getSeverityColor(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NOTIFICATION_SEVERITY::INFO:
        return QColor(0,180,180);
    case NOTIFICATION_SEVERITY::WARNING:
        return QColor(255,200,0);
    case NOTIFICATION_SEVERITY::ERROR:
        return QColor(255,50,50);
    default:
        return Qt::white;
    }
}


/**
 * @brief getSeverityColorStr
 * @param severity
 * @return
 */
QString getSeverityColorStr(NOTIFICATION_SEVERITY severity)
{
    switch (severity) {
    case NOTIFICATION_SEVERITY::INFO:
        return "rgb(0,180,180)";
    case NOTIFICATION_SEVERITY::WARNING:
        return "rgb(255,200,0)";
    case NOTIFICATION_SEVERITY::ERROR:
        return "rgb(255,50,50)";
    default:
        return "white";
    }
}
