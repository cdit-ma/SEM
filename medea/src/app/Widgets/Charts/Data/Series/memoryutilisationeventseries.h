#ifndef MEMORYUTILISATIONEVENTSERIES_H
#define MEMORYUTILISATIONEVENTSERIES_H

#include <cfloat>
#include "eventseries.h"
#include "../Events/memoryutilisationevent.h"

class MemoryUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit MemoryUtilisationEventSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event) override;

    double getMinUtilisation() const;
    double getMaxUtilisation() const;

private:
    double minUtilisation_ = DBL_MAX;
    double maxUtilisation_ = 0.0;

};

#endif // MEMORYUTILISATIONEVENTSERIES_H