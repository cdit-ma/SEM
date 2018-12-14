#ifndef BASESERIES_H
#define BASESERIES_H

#include "../Events/event.h"

#include <QDateTime>
#include <QTextStream>

enum class TIMELINE_SERIES_KIND{BASE, DATA, STATE, NOTIFICATION, LINE, BAR, PORT_LIFECYCLE, WORKLOAD, CPU_UTILISATION};

static QList<TIMELINE_SERIES_KIND> GET_TIMELINE_SERIES_KINDS()
{
    return {TIMELINE_SERIES_KIND::BASE,
            TIMELINE_SERIES_KIND::DATA,
            TIMELINE_SERIES_KIND::STATE,
            TIMELINE_SERIES_KIND::NOTIFICATION,
            TIMELINE_SERIES_KIND::LINE,
            TIMELINE_SERIES_KIND::BAR,
            TIMELINE_SERIES_KIND::PORT_LIFECYCLE,
            TIMELINE_SERIES_KIND::WORKLOAD,
            TIMELINE_SERIES_KIND::CPU_UTILISATION};
}

static QString GET_TIMELINE_SERIES_KIND_STRING(TIMELINE_SERIES_KIND kind)
{
    switch (kind) {
    case TIMELINE_SERIES_KIND::BASE:
        return "Base";
    case TIMELINE_SERIES_KIND::STATE:
        return "State";
    case TIMELINE_SERIES_KIND::NOTIFICATION:
        return "Notification";
    case TIMELINE_SERIES_KIND::LINE:
        return "Line";
    case TIMELINE_SERIES_KIND::BAR:
        return "Bar";
    case TIMELINE_SERIES_KIND::PORT_LIFECYCLE:
        return "PortLifecycle";
    case TIMELINE_SERIES_KIND::WORKLOAD:
        return "Workload";
    case TIMELINE_SERIES_KIND::CPU_UTILISATION:
        return "CPUUtilisation";
    default:
        return "Data";
    }
}

namespace MEDEA {

class EventSeries : public QObject
{
    Q_OBJECT

public:
    explicit EventSeries(QObject* parent = 0, TIMELINE_SERIES_KIND kind = TIMELINE_SERIES_KIND::BASE);
    void clear();

    void addEvent(qint64 time);
    void addEvent(Event* event);
    void addEvents(QList<Event*>& events);
    const QList<Event*>& getEvents();

    const qint64& getMinTimeMS() const;
    const qint64& getMaxTimeMS() const;
    QPair<qint64, qint64> getTimeRangeMS() const;

    const TIMELINE_SERIES_KIND& getKind() const;

    virtual QString getID() const;
    virtual QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

    static QString getDefaultDisplayFormat();
    static int getDefaultNumberOfItemsToDisplay();

private:
    QList<Event*> events_;
    TIMELINE_SERIES_KIND kind_;

    qint64 minTime_;
    qint64 maxTime_;

    int eventSeriesID_;
    static int eventSeries_ID;
};

}



#endif // BASESERIES_H
