//
// Created by Cathlyn Aston on 6/3/21.
//

#ifndef GPUTEMPERATURESERIES_H
#define GPUTEMPERATURESERIES_H

#include <cfloat>
#include "eventseries.h"
#include "../Events/gputemperatureevent.h"

class GPUTemperatureSeries : MEDEA::EventSeries
{
Q_OBJECT

public:
    explicit GPUTemperatureSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event) override;

    double getMinTemperature() const;
    double getMaxTemperature() const;

private:
    double min_temperature_ = DBL_MAX;
    double max_temperature_ = 0.0;
};

#endif // GPUTEMPERATURESERIES_H
