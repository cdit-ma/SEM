#ifndef CPUUTILISATIONEVENT_H
#define CPUUTILISATIONEVENT_H

#include "event.h"

class CPUUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit CPUUtilisationEvent(Node node, double utilisation, qint64 time, QObject* parent = 0);

    double getUtilisation() const;

    QString getHostname() const;
    QString getIp() const;

private:
    Node node_;
    double utilisation_;
};

#endif // CPUUTILISATIONEVENT_H
