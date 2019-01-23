#ifndef CPUUTILISATIONEVENTSERIES_H
#define CPUUTILISATIONEVENTSERIES_H

#include "eventseries.h"

class CPUUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit CPUUtilisationEventSeries(QString ID, QObject* parent = 0);

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());
};

#endif // CPUUTILISATIONEVENTSERIES_H
