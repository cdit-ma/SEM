#ifndef MARKERSETDATA_H
#define MARKERSETDATA_H

#include "protomessagestructs.h"
#include "Events/markerevent.h"
#include "Series/markereventseries.h"

// TODO - Not essential to Pulse, but will need to be finalised when charts (or other) start using this class to get MarkerEvents

class MarkerSetData {

public:
    MarkerSetData(const QString& marker_name);

    int getID() const;
    const QString& getMarkerName() const;

    void addMarkerEvents(const QVector<MarkerEvent*>& events);
    MarkerEventSeries* getMarkerEventSeries() const;

private:
    QString marker_name_;
    MarkerEventSeries* marker_event_series_ = nullptr;

    int marker_set_id_ = -1;
    static std::atomic<int> marker_set_id;
};

#endif // MARKERSETDATA_H
