#ifndef NETWORKUTILISATIONEVENT_H
#define NETWORKUTILISATIONEVENT_H

#include "event.h"

class NetworkUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit NetworkUtilisationEvent(const QString& hostname,
                                    double utilisation,
                                    qint64 time,
                                    QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getID() const;
    const QString& getHostname() const;

    double getUtilisation() const;

private:
    const QString hostname_;
    const double utilisation_;

};

#endif // NETWORKUTILISATIONEVENT_H
