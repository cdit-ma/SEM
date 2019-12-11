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
                                 QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getSeriesID() const;
    const QString& getID() const;
    const QString& getHostname() const;

    double getUtilisation() const;

private:
    QString series_id_;
    QString id_;
    QString hostname_;

    double utilisation_;
};

#endif // CPUUTILISATIONEVENT_H
