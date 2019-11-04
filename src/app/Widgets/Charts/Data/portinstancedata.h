#ifndef PORTINSTANCEDATA_H
#define PORTINSTANCEDATA_H

#include "protomessagestructs.h"
#include "Events/portlifecycleevent.h"
#include "Series/portlifecycleeventseries.h"
#include "Events/portevent.h"
#include "Series/porteventseries.h"

class PortInstanceData {

public:
    PortInstanceData(const AggServerResponse::Port& port);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    const QString& getPath() const;
    const QString& getMiddleware() const;

    AggServerResponse::Port::Kind getKind() const;

    void addPortLifecycleEvents(const QVector<PortLifecycleEvent*>& events);
    PortLifecycleEventSeries* getPortLifecycleEventSeries() const;

    void addPortEvents(const QVector<PortEvent*>& events);
    PortEventSeries* getPortEventSeries() const;

private:
    QString graphml_id_;
    QString name_;
    QString path_;
    QString middleware_;

    AggServerResponse::Port::Kind kind_;

    PortLifecycleEventSeries* port_lifecycle_series_ = nullptr;
    PortEventSeries* port_event_series_ = nullptr;
};

#endif // PORTINSTANCEDATA_H
