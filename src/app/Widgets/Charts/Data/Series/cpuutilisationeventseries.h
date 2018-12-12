#ifndef CPUUTILISATIONEVENTSERIES_H
#define CPUUTILISATIONEVENTSERIES_H

#include "eventseries.h"

class CPUUtilisationEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit CPUUtilisationEventSeries(QString hostname, QObject* parent = 0);

    const QString& getHostname() const;

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

private:
    QString hostname_;
};

#endif // CPUUTILISATIONEVENTSERIES_H
