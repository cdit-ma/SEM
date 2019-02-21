#ifndef MEMORYUTILISATIONEVENTSERIES_H
#define MEMORYUTILISATIONEVENTSERIES_H

#include "eventseries.h"

class MemoryUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit MemoryUtilisationEventSeries(QString ID, QObject* parent = 0);

    void addEvent(MEDEA::Event* event);

    double getMaxUtilisation();

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

private:
    double maxUtilisation_ = DBL_MIN;
};

#endif // MEMORYUTILISATIONEVENTSERIES_H
