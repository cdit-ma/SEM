#ifndef NETWORKUTILISATIONEVENTSERIES_H
#define NETWORKUTILISATIONEVENTSERIES_H

#include <cfloat>
#include "eventseries.h"
#include "../Events/networkutilisationevent.h"

class NetworkUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit NetworkUtilisationEventSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event);

    double getMaxUtilisation() const;

private:
    double minUtilisation_ = DBL_MAX;
    double maxUtilisation_ = DBL_MIN;

};

#endif // NETWORKUTILISATIONEVENTSERIES_H
