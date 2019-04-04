#include "request.h"


/**
 * @brief Request::Request
 * @param kind
 */
Request::Request(MEDEA::ChartDataKind kind) : kind_(kind) {}


/**
 * @brief Request::setExperimentID
 * @param experiment_run_id
 */
void Request::setExperimentID(const quint32 experiment_run_id)
{
    experimentRunID_ = experiment_run_id;
}


/**
 * @brief Request::setTimeInterval
 * @param time_interval
 */
void Request::setTimeInterval(const QVector<qint64> &time_interval)
{
    timeInterval_ = time_interval;
}


/**
 * @brief Request::experiment_run_id
 * @return
 */
const quint32 Request::experiment_run_id() const
{
    return experimentRunID_;
}


/**
 * @brief Request::time_interval
 * @return
 */
const QVector<qint64> &Request::time_interval() const
{
    return timeInterval_;
}


/**
 * @brief Request::getKind
 * @return
 */
const MEDEA::ChartDataKind Request::getKind() const
{
    return kind_;
}
