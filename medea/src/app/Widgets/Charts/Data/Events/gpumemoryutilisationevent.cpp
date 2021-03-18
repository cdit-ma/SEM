//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gpumemoryutilisationevent.h"

/**
 * @brief GPUMemoryUtilisationEvent::GPUMemoryUtilisationEvent
 * @param hostname
 * @param device_index
 * @param utilisation
 * @param time
 * @param parent
 */
GPUMemoryUtilisationEvent::GPUMemoryUtilisationEvent(const QString& hostname,
                                                     qint32 device_index,
                                                     double utilisation,
                                                     qint64 time,
                                                     QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_MEMORY_UTILISATION, time, parent),
      id_(hostname + QString::number(device_index) + QString::number(time)),
      hostname_(hostname),
      device_index_(device_index),
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
 * @brief GPUMemoryUtilisationEvent::getDeviceIndex
 * @return
 */
qint32 GPUMemoryUtilisationEvent::getDeviceIndex() const
{
    return device_index_;
}

/**
 * @brief GPUMemoryUtilisationEvent::getValue
 * @return
 */
double GPUMemoryUtilisationEvent::getValue() const
{
    return utilisation_;
}