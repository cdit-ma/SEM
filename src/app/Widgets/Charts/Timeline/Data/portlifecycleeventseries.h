#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "portlifecycleevent.h"

#include <QObject>

class PortLifecycleEventSeries : public QObject
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(QObject* parent = 0);

    int getID();

    void addPortEvent(PortLifecycleEvent* event);
    void addPortEvents(QList<PortLifecycleEvent*>& events);

    const QList<PortLifecycleEvent*>& getConstPortEvents();

    QPair<quint64, quint64> getRange();

private:
    QList<PortLifecycleEvent*> portEvents_;

    int ID_;
    quint64 minTime_;
    quint64 maxTime_;

    static int series_ID;

};

#endif // PORTLIFECYCLEEVENTSERIES_H
