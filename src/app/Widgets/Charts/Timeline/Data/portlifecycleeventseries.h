#ifndef PORTLIFECYCLESERIES_H
#define PORTLIFECYCLESERIES_H

#include "baseseries.h"
#include "portlifecycleevent.h"


class PortLifeCycleSeries : public BaseSeries
{
    Q_OBJECT

public:
    explicit PortLifeCycleSeries(QObject* parent = 0);
    //explicit PortLifeCycleSeries(int ID, QObject* parent = 0);

    int getID();

    void addPortEvent(PortLifeCycle* event);
    void addPortEvents(QList<PortLifeCycle*> events);

    const QList<PortLifeCycle*>& getConstPortEvents();

private:
    QList<PortLifeCycle*> portEvents_;
    int ID_;

    static int series_ID;

};

#endif // PORTLIFECYCLESERIES_H
