//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gputemperatureevent.h"

/**
 * @brief GPUTemperatureEvent::GPUTemperatureEvent
 * @param gpu_name
 * @param vendor
 * @param temperature
 * @param time
 * @param parent
 */
GPUTemperatureEvent::GPUTemperatureEvent(const QString& gpu_name,
                                         const QString& vendor,
                                         double temperature,
                                         qint64 time,
                                         QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_TEMPERATURE, time, parent),
      id_(gpu_name + QString::number(time)),
      gpu_name_(gpu_name),
      vendor_(vendor),
      temperature_(temperature) {}

/**
 * @brief GPUTemperatureEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString GPUTemperatureEvent::toString(const QString& dateTimeFormat) const
{
    return "Name: " + gpu_name_ + "\n" +
           "Vendor: " + vendor_ + "\n" +
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
 * @brief GPUTemperatureEvent::getGPUName
 * @return
 */
const QString& GPUTemperatureEvent::getGPUName() const
{
    return gpu_name_;
}

/**
 * @brief GPUTemperatureEvent::getVendor
 * @return
 */
const QString& GPUTemperatureEvent::getVendor() const
{
    return vendor_;
}

/**
 * @brief GPUTemperatureEvent::getValue
 * @return
 */
double GPUTemperatureEvent::getValue() const
{
    return temperature_;
}
