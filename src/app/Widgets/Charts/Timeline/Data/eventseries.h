#ifndef MEDEAEVENTSERIES_H
#define MEDEAEVENTSERIES_H

#include "event.h"

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

#endif // MEDEAEVENTSERIES_H
