//
// Created by Cathlyn Aston on 6/3/21.
//

#include "gputemperatureseries.h"
#include <stdexcept>

/**
 * @brief GPUTemperatureSeries::GPUTemperatureSeries
 * @param ID
 * @param parent
 */
GPUTemperatureSeries::GPUTemperatureSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::GPU_TEMPERATURE, parent) {}

/**
 * @brief GPUTemperatureSeries::addEvent
 * @param event
 */
void GPUTemperatureSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("GPUTemperatureSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::GPU_TEMPERATURE) {
        throw std::invalid_argument("GPUTemperatureSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {
        auto temperature = qobject_cast<GPUTemperatureEvent *>(event)->getValue();
        if (temperature < min_temperature_) {
            min_temperature_ = temperature;
            emit minYValueChanged(temperature);
        }
        if (temperature > max_temperature_) {
            max_temperature_ = temperature;
            emit maxYValueChanged(temperature);
        }
        addEventToList(*event);
    }
}

/**
 * @brief GPUTemperatureSeries::getMinTemperature
 * @return
 */
double GPUTemperatureSeries::getMinTemperature() const
{
    return min_temperature_;
}

/**
 * @brief GPUTemperatureSeries::getMaxTemperature
 * @return
 */
double GPUTemperatureSeries::getMaxTemperature() const
{
    return max_temperature_;
}
