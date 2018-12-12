#ifndef PORTLIFECYCLEEVENT_H
#define PORTLIFECYCLEEVENT_H

#include "event.h"

class PortLifecycleEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit PortLifecycleEvent(Port port, LifecycleType type, qint64 time, QObject* parent = 0);

    const Port& getPort() const;
    const LifecycleType& getType() const;

    QString getEventID() const;

private:    
    Port port_;
    LifecycleType type_;

};

#endif // PORTLIFECYCLEEVENT_H
