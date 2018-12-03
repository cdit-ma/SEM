#ifndef BASESERIES_H
#define BASESERIES_H

#include "../Events/event.h"

enum class TIMELINE_SERIES_KIND{BASE, DATA, STATE, NOTIFICATION, LINE, BAR, EVENT};

namespace MEDEA {

class EventSeries : public QObject
{
    Q_OBJECT

public:
    explicit EventSeries(QObject* parent = 0, TIMELINE_SERIES_KIND kind = TIMELINE_SERIES_KIND::BASE);
    void clear();

    void addEvent(MEDEA::Event* event);
    void addEvents(QList<MEDEA::Event*>& events);
    const QList<MEDEA::Event*>& getEvents() const;

    const qint64& getMinTimeMS() const;
    const qint64& getMaxTimeMS() const;
    QPair<qint64, qint64> getTimeRangeMS() const;

    const TIMELINE_SERIES_KIND& getKind() const;

    virtual QString getID() const;
    virtual QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, QString displayFormat = "hh:mm:ss:zz");

private:
    QList<MEDEA::Event*> events_;

    TIMELINE_SERIES_KIND kind_;

    qint64 minTime_;
    qint64 maxTime_;

    int eventSeriesID_;
    static int eventSeries_ID;
};

}



#endif // BASESERIES_H
