#ifndef MEMORYUTILISATIONEVENTSERIES_H
#define MEMORYUTILISATIONEVENTSERIES_H

#include "eventseries.h"

class MemoryUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit MemoryUtilisationEventSeries(QString ID, QObject* parent = 0);

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());
};

#endif // MEMORYUTILISATIONEVENTSERIES_H
