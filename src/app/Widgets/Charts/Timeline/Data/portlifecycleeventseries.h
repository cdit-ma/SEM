<<<<<<< HEAD
#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "portlifecycleevent.h"

#include <QObject>

class PortLifecycleEventSeries : public QObject
=======
#ifndef PORTLIFECYCLESERIES_H
#define PORTLIFECYCLESERIES_H

#include "baseseries.h"
#include "portlifecycleevent.h"


class PortLifeCycleSeries : public BaseSeries
>>>>>>> feature-hub
{
    Q_OBJECT

public:
<<<<<<< HEAD
    explicit PortLifecycleEventSeries(QString path, QObject* parent = 0);

    QString getPortPath();

private:
    QString port_path;

};

#endif // PORTLIFECYCLEEVENTSERIES_H
=======
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
>>>>>>> feature-hub
