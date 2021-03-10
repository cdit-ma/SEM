//
// Created by Cathlyn Aston on 5/3/21.
//

#include "gpucomputeutilisationevent.h"

/**
 * @brief GPUComputeUtilisationEvent::
 * @param gpu_name
 * @param vendor
 * @param utilisation
 * @param time
 * @param parent
 */
GPUComputeUtilisationEvent::GPUComputeUtilisationEvent(const QString& gpu_name,
                                                       const QString& vendor,
                                                       double utilisation,
                                                       qint64 time,
                                                       QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::GPU_COMPUTE_UTILISATION, time, parent),
      id_(gpu_name + QString::number(time)),
      gpu_name_(gpu_name),
      vendor_(vendor),
      utilisation_(utilisation) {}

/**
 * @brief GPUComputeUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString GPUComputeUtilisationEvent::toString(const QString &dateTimeFormat) const
{
    return "Name: " + gpu_name_ + "\n" +
           "Vendor: " + vendor_ + "\n" +
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
 * @brief GPUComputeUtilisationEvent::getGPUName
 * @return
 */
const QString& GPUComputeUtilisationEvent::getGPUName() const
{
    return gpu_name_;
}

/**
 * @brief GPUComputeUtilisationEvent::getVendor
 * @return
 */
const QString& GPUComputeUtilisationEvent::getVendor() const
{
    return vendor_;
}

/**
 * @brief GPUComputeUtilisationEvent::getValue
 * @return
 */
double GPUComputeUtilisationEvent::getValue() const
{
    return utilisation_;
}