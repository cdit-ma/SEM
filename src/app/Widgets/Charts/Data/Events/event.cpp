#include "event.h"
#include <QDateTime>

int MEDEA::Event::event_ID = 0;

/**
 * @brief MEDEA::Event::Event
 * @param kind
 * @param time
 * @param name
 * @param parent
 */
MEDEA::Event::Event(MEDEA::ChartDataKind kind, qint64 time, const QString& name, QObject *parent)
    : QObject(parent),
      kind_(kind),
      time_(time),
      series_name_(name),
      event_id_(event_ID++) {}


/**
 * @brief MEDEA::Event::getKind
 * @return
 */
MEDEA::ChartDataKind MEDEA::Event::getKind() const
{
    return kind_;
}


/**
 * @brief MEDEA::Event::getTimeMS
 * @return
 */
qint64 MEDEA::Event::getTimeMS() const
{
    return time_;
}


/**
 * @brief MEDEA::Event::getDateTimeString
 * @param format
 * @return
 */
QString MEDEA::Event::getDateTimeString(const QString& format) const
{
    return QDateTime::fromMSecsSinceEpoch(time_).toString(format);
}


/**
 * @brief MEDEA::Event::getName
 * @return
 */
const QString& MEDEA::Event::getSeriesName() const
{
    return series_name_;
}


/**
 * @brief MEDEA::Event::GetChartDataKinds
 * @return
 */
const QList<MEDEA::ChartDataKind> &MEDEA::Event::GetChartDataKinds()
{
    static const QList<ChartDataKind> kinds {
                ChartDataKind::DATA,
                ChartDataKind::PORT_LIFECYCLE,
                ChartDataKind::WORKLOAD,
                ChartDataKind::CPU_UTILISATION,
                ChartDataKind::MEMORY_UTILISATION,
                ChartDataKind::MARKER,
                ChartDataKind::PORT_EVENT
    };
    return kinds;
}


/**
 * @brief MEDEA::Event::GetChartDataKindString
 * @param kind
 * @return
 */
const QString &MEDEA::Event::GetChartDataKindString(MEDEA::ChartDataKind kind)
{
    switch (kind) {
    case ChartDataKind::PORT_LIFECYCLE: {
        static const QString portLifecycleStr = "PortLifecycle";
        return portLifecycleStr;
    }
    case ChartDataKind::WORKLOAD: {
        static const QString workloadStr = "Workload";
        return workloadStr;
    }
    case ChartDataKind::CPU_UTILISATION: {
        static const QString cpuStr = "CPUUtilisation";
        return cpuStr;
    }
    case ChartDataKind::MEMORY_UTILISATION: {
        static const QString memoryStr = "MemoryUtilisation";
        return memoryStr;
    }
    case ChartDataKind::MARKER: {
        static const QString markerStr = "Marker";
        return markerStr;
    }
    case ChartDataKind::PORT_EVENT: {
        static const QString portEventStr = "PortEvent";
        return portEventStr;
    }
    default: {
        static const QString defaultStr = "Data";
        return defaultStr;
    }
    }
}


/**
 * @brief MEDEA::Event::GetChartDataKindStringSuffix
 * @param kind
 * @return
 */
const QString &MEDEA::Event::GetChartDataKindStringSuffix(MEDEA::ChartDataKind kind)
{
    switch (kind) {
    case ChartDataKind::PORT_LIFECYCLE: {
        static const QString lifecycleSuffix = "_lifecycle";
        return lifecycleSuffix;
    }
    case ChartDataKind::CPU_UTILISATION: {
        static const QString cpuSuffix = "_cpu";
        return cpuSuffix;
    }
    case ChartDataKind::MEMORY_UTILISATION: {
        static const QString memorySuffix = "_mem";
        return memorySuffix;
    }
    default: {
        static const QString defaultSuffix = "";
        return defaultSuffix;
    }
    }
}


