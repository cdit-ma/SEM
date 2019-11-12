#ifndef CPUUTILISATIONEVENTSERIES_H
#define CPUUTILISATIONEVENTSERIES_H

#include <cfloat>
#include "eventseries.h"
#include "../Events/cpuutilisationevent.h"

class CPUUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit CPUUtilisationEventSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event);

    double getMaxUtilisation() const;

private:
    double minUtilisation_ = DBL_MAX;
    double maxUtilisation_ = DBL_MIN;

};

#endif // CPUUTILISATIONEVENTSERIES_H
