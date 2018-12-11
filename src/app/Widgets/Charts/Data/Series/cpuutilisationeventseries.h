#ifndef CPUUTILISATIONEVENTSERIES_H
#define CPUUTILISATIONEVENTSERIES_H

#include "eventseries.h"

class CPUUtilisationEventSeries : MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit CPUUtilisationEventSeries(QString hostname, QString ip, QObject* parent = 0);

    const QString& getHostname() const;
    const QString& getIp() const;

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

private:
    QString hostname_;
    QString ip_;
};

#endif // CPUUTILISATIONEVENTSERIES_H
