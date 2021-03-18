//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gpucomputeutilisationevent.h"

/**
 * @brief GPUComputeUtilisationEvent::
 * @param hostname
 * @param device_index
 * @param utilisation
 * @param time
 * @param parent
 */
GPUComputeUtilisationEvent::GPUComputeUtilisationEvent(const QString& hostname,
                                                       qint32 device_index,
                                                       double utilisation,
                                                       qint64 time,
                                                       QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_COMPUTE_UTILISATION, time, parent),
      id_(hostname + QString::number(device_index) + QString::number(time)),
      hostname_(hostname),
      device_index_(device_index),
      utilisation_(utilisation) {}

/**
 * @brief GPUComputeUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString GPUComputeUtilisationEvent::toString(const QString &dateTimeFormat) const
{
    return "Hostname: " + hostname_ + "\n" +
           "Utilisation: " + QString::number(utilisation_ * 100) + "%\n" +
           "At " + getDateTimeString(dateTimeFormat) + "\n\n";
}

/**
 * @brief GPUComputeUtilisationEvent::getID
 * @return
 */
const QString& GPUComputeUtilisationEvent::getID() const
{
    return id_;
}

/**
 * @brief GPUComputeUtilisationEvent::getHostname
 * @return
 */
const QString& GPUComputeUtilisationEvent::getHostname() const
{
    return hostname_;
}

/**
 * @brief GPUComputeUtilisationEvent::getDeviceIndex
 * @return
 */
qint32 GPUComputeUtilisationEvent::getDeviceIndex() const
{
    return device_index_;
}

/**
 * @brief GPUComputeUtilisationEvent::getValue
 * @return
 */
double GPUComputeUtilisationEvent::getValue() const
{
    return utilisation_;
}