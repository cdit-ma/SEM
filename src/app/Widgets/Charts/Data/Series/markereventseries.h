#ifndef MARKEREVENTSERIES_H
#define MARKEREVENTSERIES_H

#include "eventseries.h"

class MarkerEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit MarkerEventSeries(QString ID, QObject* parent = 0);

    const QHash<qint64, QPair<qint64, qint64>>& getMarkerIDSetRanges() const;
    const QMap<qint64, QSet<qint64>>& getMarkerIDsWithSharedStartTimes() const;
    const QHash<qint64, qint64>& getMarkerIDSetDurations() const;

    void addEvent(MEDEA::Event *event);
    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat);

private:
    QString getElapsedTimeString(qint64 fromMS, qint64 toMS) const;

    QHash<qint64, QPair<qint64, qint64>> markerIDSetRanges_;
    QHash<qint64, qint64> markerIDSetDurations_;
    QMap<qint64, QSet<qint64>> startTimeMap_;
    //QMultiHash<qint64, qint64> startTimeMultiHash_;

};

#endif // MARKEREVENTSERIES_H
