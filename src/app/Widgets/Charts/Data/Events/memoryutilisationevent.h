#ifndef MEMORYUTILISATIONEVENT_H
#define MEMORYUTILISATIONEVENT_H

#include "event.h"

class MemoryUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit MemoryUtilisationEvent(QString hostname, double utilisation, qint64 time, QObject* parent = 0);

    const double& getUtilisation() const;
    const QString& getHostname() const;

    QString getID() const;
    TIMELINE_EVENT_KIND getKind()const ;

private:
    QString hostname_;
    double utilisation_;

};

#endif // MEMORYUTILISATIONEVENT_H
