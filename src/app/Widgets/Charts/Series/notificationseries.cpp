#include "notificationseries.h"


/**
 * @brief MEDEA::NotificationSeries::NotificationSeries
 * @param item
 */
MEDEA::NotificationSeries::NotificationSeries(ViewItem* item)
    : DataSeries(item, TIMELINE_SERIES_KIND::NOTIFICATION)
{
}


/**
 * @brief MEDEA::NotificationSeries::~NotificationSeries
 */
MEDEA::NotificationSeries::~NotificationSeries()
{
}


/**
 * @brief MEDEA::NotificationSeries::addTime
 * @param time
 */
void MEDEA::NotificationSeries::addTime(double time, NOTIFICATION_TYPE type)
{
    _times.append(QPair<double, MEDEA::NotificationSeries::NOTIFICATION_TYPE>{time, type});
    addPoint(QPointF(time, (int) type));
}


/**
 * @brief MEDEA::NotificationSeries::getTimes
 * @return
 */
QList<QPair<double, MEDEA::NotificationSeries::NOTIFICATION_TYPE>> MEDEA::NotificationSeries::getTimes()
{
     return _times;
}
