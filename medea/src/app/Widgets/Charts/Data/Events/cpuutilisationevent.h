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

    QString toString(const QString& dateTimeFormat) const override;

    const QString& getID() const override;
    const QString& getHostname() const;

    double getValue() const;

private:
    QString id_;
    QString hostname_;

    double utilisation_;
};

#endif // CPUUTILISATIONEVENT_H