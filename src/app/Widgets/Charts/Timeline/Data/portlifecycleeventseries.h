#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "portlifecycleevent.h"

#include <QObject>

class PortLifecycleEventSeries : public QObject
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(QString path, QObject* parent = 0);

    QString getPortPath();

private:
    QString port_path;

};

#endif // PORTLIFECYCLEEVENTSERIES_H
