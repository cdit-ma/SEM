#ifndef CPUUTILISATIONEVENT_H
#define CPUUTILISATIONEVENT_H

#include "event.h"

class CPUUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit CPUUtilisationEvent(const QString& hostname,
                                 double utilisation,
                                 qint64 time,
                                 QObject* parent = 0);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getID() const;
    const QString& getHostname() const;

    double getUtilisation() const;

private:
    const QString hostname_;
    const double utilisation_;

};

#endif // CPUUTILISATIONEVENT_H
