#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "eventseries.h"
#include "../Events/portlifecycleevent.h"


class PortLifecycleEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(QString path, QObject* parent = 0);

    const QString& getPortPath() const;
    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

private:
    QString getTypeString(LifecycleType type);

    QString port_path;

};

#endif // PORTLIFECYCLEEVENTSERIES_H
