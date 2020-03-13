#ifndef MARKEREVENTSERIES_H
#define MARKEREVENTSERIES_H

#include "eventseries.h"
#include "../Events/markerevent.h"

#include <QHash>
#include <QMap>

class MarkerEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit MarkerEventSeries(const QString& ID, QObject* parent = nullptr);

    const QMap<qint64, QSet<qint64>>& getMarkerIDsMappedByStartTimes() const;
    const QHash<qint64, QPair<qint64, qint64>>& getMarkerIDSetRanges() const;
    const QHash<qint64, qint64>& getMarkerIDSetDurations() const;

    QString getHoveredDataString (
            qint64 fromTimeMS,
            qint64 toTimeMS,
            int numberOfItemsToDisplay,
            const QString& displayFormat) const override;

    void addEvent(MEDEA::Event* event) override;

protected:
    QString getDataString (
            const QList<qint64>::const_iterator& from,
            const QList<qint64>::const_iterator& to) const;

private:
    QHash<qint64, QPair<qint64, qint64>> markerIDSetRanges_;
    QHash<qint64, qint64> markerIDSetDurations_;
    QMap<qint64, QSet<qint64>> startTimeMap_;
};

#endif // MARKEREVENTSERIES_H