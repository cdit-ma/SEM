#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "portlifecycleevent.h"

#include <QObject>

class PortLifecycleEventSeries : public QObject
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(QString path, QObject* parent = 0);

    void addPortEvent(PortLifecycleEvent* event);
    void addPortEvents(QList<PortLifecycleEvent*>& events);

    const QList<PortLifecycleEvent*>& getConstPortEvents();

    QString getPortPath();
    QPair<qint64, qint64> getRange();

private:
    QList<PortLifecycleEvent*> portEvents_;

    QString port_path;
    qint64 minTime_mu;
    qint64 maxTime_mu;

    static int series_ID;

};

#endif // PORTLIFECYCLEEVENTSERIES_H
