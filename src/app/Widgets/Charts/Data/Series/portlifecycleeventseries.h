#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "eventseries.h"

class PortLifecycleEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(QString ID, QObject* parent = 0);

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

private:
    QString getTypeString(AggServerResponse::LifecycleType type);
};

#endif // PORTLIFECYCLEEVENTSERIES_H
