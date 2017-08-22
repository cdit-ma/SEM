#include "notificationEnumerations.h"
#include "../../theme.h"


extern QList<NOTIFICATION_CONTEXT> getNotificationContexts(){
    return {
        NOTIFICATION_CONTEXT::SELECTED,
        NOTIFICATION_CONTEXT::NOT_SELECTED
    };
}

/**
 * @brief getNotificationTypes
 * @return
 */
QList<NOTIFICATION_TYPE> getNotificationTypes()
{
    return {
        NOTIFICATION_TYPE::MODEL,
        NOTIFICATION_TYPE::APPLICATION
    };
}


/**
 * @brief getNotificationCategories
 * @return
 */
QList<NOTIFICATION_CATEGORY> getNotificationCategories()
{
    return {
        NOTIFICATION_CATEGORY::NONE,
        NOTIFICATION_CATEGORY::FILE,
        NOTIFICATION_CATEGORY::JENKINS,
        NOTIFICATION_CATEGORY::VALIDATION
    };
}


/**
 * @brief getNotificationSeverities
 * @return
 */
QList<NOTIFICATION_SEVERITY> getNotificationSeverities()
{
    return {
        NOTIFICATION_SEVERITY::INFO,
        NOTIFICATION_SEVERITY::WARNING,
        NOTIFICATION_SEVERITY::ERROR
    };
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

QString getContextString(NOTIFICATION_CONTEXT context)
{
    switch (context) {
    case NOTIFICATION_CONTEXT::SELECTED:
        return "Selected";
    case NOTIFICATION_CONTEXT::NOT_SELECTED:
        return "Not Selected";
    default:
        return "Unknown Context";
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
 * @brief getSeverityIcongetSeverityIcon
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
        return "pointyCircleCriticalDark";
    default:
        return "circleQuestion";
    }
}
QString getContextIcon(NOTIFICATION_CONTEXT context){
    switch (context) {
        default:
            return "tiles" ;
    }
}

QString getTypeIcon(NOTIFICATION_TYPE type){
    switch (type) {
    case NOTIFICATION_TYPE::MODEL:
        return "dotsInRectangle";
    case NOTIFICATION_TYPE::APPLICATION:
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
    case NOTIFICATION_CATEGORY::JENKINS:
        return "jenkinsFlat";
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
        return Theme::theme()->getTextColor();
        //return QColor(0,180,180);
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
