//
// Created by Cathlyn Aston on 6/3/21.
//

#ifndef GPUCOMPUTEUTILISATIONSERIES_H
#define GPUCOMPUTEUTILISATIONSERIES_H

#include <cfloat>
#include "eventseries.h"
#include "../Events/gpucomputeutilisationevent.h"

class GPUComputeUtilisationSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit GPUComputeUtilisationSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event) override;

    double getMinUtilisation() const;
    double getMaxUtilisation() const;

private:
    double min_utilisation_ = DBL_MAX;
    double max_utilisation_ = 0.0;
};

#endif // GPUCOMPUTEUTILISATIONSERIES_H
