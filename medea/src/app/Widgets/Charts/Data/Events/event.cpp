#include "event.h"
#include <QDateTime>

/**
 * @brief MEDEA::Event::Event
 * @param kind
 * @param time
 * @param name
 * @param parent
 */
MEDEA::Event::Event(MEDEA::ChartDataKind kind, qint64 time, QObject *parent)
	: QObject(parent),
	  kind_(kind),
	  time_(time) {}

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
 * @brief MEDEA::Event::GetChartDataKinds
 * @return
 */
const QList<MEDEA::ChartDataKind>& MEDEA::Event::GetChartDataKinds()
{
    static const QList<ChartDataKind> kinds {
                ChartDataKind::DATA,
                ChartDataKind::PORT_LIFECYCLE,
                ChartDataKind::WORKLOAD,
                ChartDataKind::CPU_UTILISATION,
                ChartDataKind::MEMORY_UTILISATION,
                ChartDataKind::MARKER,
                ChartDataKind::PORT_EVENT,
                ChartDataKind::NETWORK_UTILISATION,
                ChartDataKind::GPU_COMPUTE_UTILISATION,
                ChartDataKind::GPU_MEMORY_UTILISATION,
                ChartDataKind::GPU_TEMPERATURE
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
        case ChartDataKind::NETWORK_UTILISATION: {
            static const QString networkStr = "NetworkUtilisation";
            return networkStr;
        }
        case ChartDataKind::GPU_COMPUTE_UTILISATION:
            static const QString gpuComputeStr = "GPUComputeUtilisation";
            return gpuComputeStr;
        case ChartDataKind::GPU_MEMORY_UTILISATION:
            static const QString gpuMemoryStr = "GPUMemoryUtilisation";
            return gpuMemoryStr;
        case ChartDataKind::GPU_TEMPERATURE:
            static const QString gpuTemperatureStr = "GPUTemperature";
            return gpuTemperatureStr;
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
const QString& MEDEA::Event::GetChartDataKindStringSuffix(MEDEA::ChartDataKind kind)
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
        case ChartDataKind::MEMORY_UTILISATION:
            [[falltrhough]];
        case ChartDataKind::GPU_MEMORY_UTILISATION: {
            static const QString memorySuffix = "_mem";
            return memorySuffix;
        }
        case ChartDataKind::NETWORK_UTILISATION: {
            static const QString networkSuffix = "_net";
            return networkSuffix;
        }
        case ChartDataKind::GPU_COMPUTE_UTILISATION: {
            static const QString gpuComputeSuffix = "_compute";
            return gpuComputeSuffix;
        }
        case ChartDataKind::GPU_TEMPERATURE: {
            static const QString gpuTemperatureSuffix = "_temp";
            return gpuTemperatureSuffix;
        }
        default: {
            static const QString defaultSuffix = "";
            return defaultSuffix;
        }
    }
}