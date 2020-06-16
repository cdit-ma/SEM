#ifndef MARKERSETDATA_H
#define MARKERSETDATA_H

#include "protomessagestructs.h"
#include "Requests/markerrequest.h"
#include "Events/markerevent.h"
#include "Series/markereventseries.h"

// TODO - Not essential to Pulse, but will need to be finalised when charts (or other) start using this class to get MarkerEvents

class MarkerSetData : public QObject
{
    Q_OBJECT

public:
    MarkerSetData(quint32 exp_run_id, const QString& marker_name, QObject* parent = nullptr);
    
    const QString& getMarkerName() const;
    const MarkerRequest& getMarkerRequest() const;

    void addMarkerEvents(const QVector<MarkerEvent*>& events);
    QPointer<const MEDEA::EventSeries> getMarkerEventSeries() const;

    void updateData(qint64 new_last_updated_time);

signals:
    void requestData(MarkerSetData& marker_set);

private:
    void setupSeries();

    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString marker_name_;

    MarkerRequest marker_request_;
    MarkerEventSeries* marker_event_series_ = nullptr;
};

#endif // MARKERSETDATA_H