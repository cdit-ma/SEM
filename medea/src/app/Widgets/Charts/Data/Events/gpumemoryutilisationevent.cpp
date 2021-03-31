//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gpumemoryutilisationevent.h"

/**
 * @brief GPUMemoryUtilisationEvent::GPUMemoryUtilisationEvent
 * @param hostname
 * @param device_name
 * @param utilisation
 * @param time
 * @param parent
 */
GPUMemoryUtilisationEvent::GPUMemoryUtilisationEvent(const QString& hostname,
                                                     QString device_name,
                                                     double utilisation,
                                                     qint64 time,
                                                     QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_MEMORY_UTILISATION, time, parent),
      id_(hostname + device_name + QString::number(time)),
      hostname_(hostname),
      device_name_(device_name),
      utilisation_(utilisation) {}

/**
 * @brief GPUMemoryUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString GPUMemoryUtilisationEvent::toString(const QString &dateTimeFormat) const
{
    return "Hostname: " + hostname_ + "\n" +
           "Utilisation: " + QString::number(utilisation_ * 100) + "%\n" +
           "At " + getDateTimeString(dateTimeFormat) + "\n\n";
}

/**
 * @brief GPUMemoryUtilisationEvent::getID
 * @return
 */
const QString& GPUMemoryUtilisationEvent::getID() const
{
    return id_;
}

/**
 * @bried GPUMemoryUtilisationEvent::getHostname
 * @return
 */
const QString& GPUMemoryUtilisationEvent::getHostname() const
{
    return hostname_;
}

/**
 * @brief GPUMemoryUtilisationEvent::getDeviceName
 * @return
 */
QString GPUMemoryUtilisationEvent::getDeviceName() const
{
    return device_name_;
}

/**
 * @brief GPUMemoryUtilisationEvent::getValue
 * @return
 */
double GPUMemoryUtilisationEvent::getValue() const
{
    return utilisation_;
}