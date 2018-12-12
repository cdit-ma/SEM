#ifndef CPUUTILISATIONEVENT_H
#define CPUUTILISATIONEVENT_H

#include "event.h"

class CPUUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit CPUUtilisationEvent(QString hostname, double utilisation, qint64 time, QObject* parent = 0);

    const double& getUtilisation() const;
    const QString& getHostname() const;

    QString getEventID() const;

private:
    QString hostname_;
    double utilisation_;
};

#endif // CPUUTILISATIONEVENT_H
