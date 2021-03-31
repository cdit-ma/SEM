//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gputemperatureevent.h"

/**
 * @brief GPUTemperatureEvent::GPUTemperatureEvent
 * @param hostname
 * @param device_name
 * @param temperature
 * @param time
 * @param parent
 */
GPUTemperatureEvent::GPUTemperatureEvent(const QString& hostname,
                                         QString device_name,
                                         double temperature,
                                         qint64 time,
                                         QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_TEMPERATURE, time, parent),
      id_(hostname + device_name + QString::number(time)),
      hostname_(hostname),
      device_name_(device_name),
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
 * @brief GPUTemperatureEvent::getDeviceName
 * @return
 */
QString GPUTemperatureEvent::getDeviceName() const
{
    return device_name_;
}

/**
 * @brief GPUTemperatureEvent::getValue
 * @return
 */
double GPUTemperatureEvent::getValue() const
{
    return temperature_;
}
