//
// Created by Cathlyn Aston on 6/3/21.
//

#include "gpucomputeutilisationseries.h"
#include <stdexcept>

/**
 * @brief GPUComputeUtilisationSeries::GPUComputeUtilisationSeries
 * @param ID
 * @param parent
 */
GPUComputeUtilisationSeries::GPUComputeUtilisationSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::GPU_COMPUTE_UTILISATION, parent) {}

/**
 * @brief GPUComputeUtilisationSeries::addEvent
 * @param event
 */
void GPUComputeUtilisationSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("GPUComputeUtilisationSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::GPU_COMPUTE_UTILISATION) {
       throw std::invalid_argument("GPUComputeUtilisationSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {
        auto utilisation = qobject_cast<GPUComputeUtilisationEvent*>(event)->getUtilisation();
        if (utilisation < min_utilisation_) {
            min_utilisation_ = utilisation;
            emit minYValueChanged(utilisation);
        }
        if (utilisation > max_utilisation_) {
            max_utilisation_ = utilisation;
            emit maxYValueChanged(utilisation);
        }
        addEventToList(*event);
    }
}

/**
 * @brief GPUComputeUtilisationSeries::getMinUtilisation
 * @return
 */
double GPUComputeUtilisationSeries::getMinUtilisation() const
{
    return min_utilisation_;
}

/**
 * @brief GPUComputeUtilisationSeries::getMaxUtilisation
 * @return
 */
double GPUComputeUtilisationSeries::getMaxUtilisation() const
{
    return max_utilisation_;
}