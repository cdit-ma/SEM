#ifndef PORTINSTANCEDATA_H
#define PORTINSTANCEDATA_H

#include "protomessagestructs.h"

#include "Requests/portlifecyclerequest.h"
#include "Requests/porteventrequest.h"

#include "Events/portlifecycleevent.h"
#include "Events/portevent.h"
#include "Series/portlifecycleeventseries.h"
#include "Series/porteventseries.h"

#include <QPointer>

class ComponentInstanceData;

class PortInstanceData : public QObject
{
    Q_OBJECT

public:
    PortInstanceData(quint32 exp_run_id, const ComponentInstanceData& comp_inst, const AggServerResponse::Port& port, QObject* parent = nullptr);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    const QString& getPath() const;
    const QString& getMiddleware() const;

    AggServerResponse::Port::Kind getKind() const;

    const PortLifecycleRequest& getPortLifecycleRequest() const;
    const PortEventRequest& getPortEventRequest() const;

    void addPortLifecycleEvents(const QVector<PortLifecycleEvent*>& events);
    QPointer<const MEDEA::EventSeries> getPortLifecycleSeries() const;

    void addPortEvents(const QVector<PortEvent*>& events);
    QPointer<const MEDEA::EventSeries> getPortEventSeries() const;

    qint64 getPreviousEventTime(qint64 time) const;
    qint64 getNextEventTime(qint64 time) const;

    void updateData(qint64 new_last_updated_time);

signals:
    void requestData(PortInstanceData& port);

private:
    void setupRequests();
    void setupSeries();

    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString graphml_id_;
    QString name_;
    QString path_;
    QString middleware_;

    AggServerResponse::Port::Kind kind_;

    PortLifecycleRequest port_lifecycle_request_;
    PortEventRequest port_event_request_;

    PortLifecycleEventSeries* port_lifecycle_series_ = nullptr;
    PortEventSeries* port_event_series_ = nullptr;
};

#endif // PORTINSTANCEDATA_H