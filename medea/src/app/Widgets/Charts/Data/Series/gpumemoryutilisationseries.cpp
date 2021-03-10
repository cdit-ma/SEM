//
// Created by Cathlyn Aston on 6/3/21.
//

#include "gpumemoryutilisationseries.h"
#include <stdexcept>

/**
 * @brief GPUMemoryUtilisationSeries::GPUMemoryUtilisationSeries
 * @param ID
 * @param parent
 */
GPUMemoryUtilisationSeries::GPUMemoryUtilisationSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::GPU_MEMORY_UTILISATION, parent) {}

/**
 * @brief GPUMemoryUtilisationSeries::addEvent
 * @param event
 */
void GPUMemoryUtilisationSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("GPUMemoryUtilisationSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::GPU_MEMORY_UTILISATION) {
        throw std::invalid_argument("GPUMemoryUtilisationSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {
        auto utilisation = qobject_cast<GPUMemoryUtilisationEvent *>(event)->getValue();
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
 * @brief GPUMemoryUtilisationSeries::getMinUtilisation
 * @return
 */
double GPUMemoryUtilisationSeries::getMinUtilisation() const
{
    return min_utilisation_;
}

/**
 * @brief GPUMemoryUtilisationSeries::getMaxUtilisation
 * @return
 */
double GPUMemoryUtilisationSeries::getMaxUtilisation() const
{
    return max_utilisation_;
}
