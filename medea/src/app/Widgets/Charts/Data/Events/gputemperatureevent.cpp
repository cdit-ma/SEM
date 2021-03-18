//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gputemperatureevent.h"

/**
 * @brief GPUTemperatureEvent::GPUTemperatureEvent
 * @param hostname
 * @param device_index
 * @param temperature
 * @param time
 * @param parent
 */
GPUTemperatureEvent::GPUTemperatureEvent(const QString& hostname,
                                         qint32 device_index,
                                         double temperature,
                                         qint64 time,
                                         QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_TEMPERATURE, time, parent),
      id_(hostname + QString::number(device_index) + QString::number(time)),
      hostname_(hostname),
      device_index_(device_index),
      temperature_(temperature) {}

/**
 * @brief GPUTemperatureEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString GPUTemperatureEvent::toString(const QString& dateTimeFormat) const
{
    return "Hostname: " + hostname_ + "\n" +
           "Temperature: " + temperature_ + "°C\n" +
           "At " + getDateTimeString(dateTimeFormat) + "\n\n";
}

/**
 * @brief GPUTemperatureEvent::getID
 * @return
 */
const QString& GPUTemperatureEvent::getID() const
{
    return id_;
}

/**
 * @brief GPUTemperatureEvent::getHostname
 * @return
 */
const QString& GPUTemperatureEvent::getHostname() const
{
    return hostname_;
}

/**
 * @brief GPUTemperatureEvent::getDeviceIndex
 * @return
 */
qint32 GPUTemperatureEvent::getDeviceIndex() const
{
    return device_index_;
}

/**
 * @brief GPUTemperatureEvent::getValue
 * @return
 */
double GPUTemperatureEvent::getValue() const
{
    return temperature_;
}
