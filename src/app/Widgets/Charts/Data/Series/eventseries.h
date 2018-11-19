#ifndef BASESERIES_H
#define BASESERIES_H

#include "../Events/event.h"

namespace MEDEA {

class EventSeries : public QObject
{
    Q_OBJECT

public:
    explicit EventSeries(QObject* parent = 0);

    void addEvent(MEDEA::Event* event);
    const QList<MEDEA::Event*>& getEvents();

    virtual QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS);

private:
    QList<MEDEA::Event*> events_;
};

}



#endif // BASESERIES_H
