//
// Created by Cathlyn Aston on 6/3/21.
//

#ifndef GPUMEMORYUTILISATIONSERIES_H
#define GPUMEMORYUTILISATIONSERIES_H

#include <cfloat>
#include "eventseries.h"
#include "../Events/gpumemoryutilisationevent.h"

class GPUMemoryUtilisationSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit GPUMemoryUtilisationSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event) override;

    double getMinUtilisation() const;
    double getMaxUtilisation() const;

private:
    double min_utilisation_ = DBL_MAX;
    double max_utilisation_ = 0.0;
};

#endif // GPUMEMORYUTILISATIONSERIES_H
