#ifndef BASESERIES_H
#define BASESERIES_H

#include "../Events/event.h"

namespace MEDEA {

class EventSeries : public QObject
{
    Q_OBJECT

public:
    explicit EventSeries(QObject* parent = 0);
    void clear();

    void addEvent(MEDEA::Event* event);
    void addEvents(QList<MEDEA::Event*>& events);
    const QList<MEDEA::Event*>& getEvents();

    qint64 getMinTimeMS();
    qint64 getMaxTimeMS();
    QPair<qint64, qint64> getTimeRangeMS();

    virtual QString getID();
    virtual QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, QString displayFormat = "hh:mm:ss:zz");

private:
    QList<MEDEA::Event*> events_;

    qint64 minTime_;
    qint64 maxTime_;
    int eventSeriesID_;

    static int eventSeries_ID;
};

}



#endif // BASESERIES_H
