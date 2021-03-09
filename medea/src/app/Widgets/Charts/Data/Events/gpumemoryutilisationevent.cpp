//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gpumemoryutilisationevent.h"

/**
 * @brief GPUMemoryUtilisationEvent::GPUMemoryUtilisationEvent
 * @param gpu_name
 * @param vendor
 * @param utilisation
 * @param time
 * @param parent
 */
GPUMemoryUtilisationEvent::GPUMemoryUtilisationEvent(const QString& gpu_name,
                                                     const QString& vendor,
                                                     double utilisation,
                                                     qint64 time,
                                                     QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_MEMORY_UTILISATION, time, parent),
      id_(gpu_name + QString::number(time)),
      gpu_name_(gpu_name),
      vendor_(vendor),
      utilisation_(utilisation) {}

/**
 * @brief GPUMemoryUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString GPUMemoryUtilisationEvent::toString(const QString &dateTimeFormat) const
{
    return "Name: " + gpu_name_ + "\n" +
           "Vendor: " + vendor_ + "\n" +
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
 * @bried GPUMemoryUtilisationEvent::getGPUName
 * @return
 */
const QString& GPUMemoryUtilisationEvent::getGPUName() const
{
    return gpu_name_;
}

/**
 * @brief GPUMemoryUtilisationEvent::getVendor
 * @return
 */
const QString& GPUMemoryUtilisationEvent::getVendor() const
{
    return vendor_;
}

/**
 * @brief GPUMemoryUtilisationEvent::getUtilisation
 * @return
 */
double GPUMemoryUtilisationEvent::getUtilisation() const
{
    return utilisation_;
}