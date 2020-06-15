#ifndef MEMORYUTILISATIONEVENT_H
#define MEMORYUTILISATIONEVENT_H

#include "event.h"

class MemoryUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit MemoryUtilisationEvent(const QString& hostname,
                                    double utilisation,
                                    qint64 time,
                                    QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const override;

    const QString& getID() const override;
    const QString& getHostname() const;

    double getUtilisation() const;

private:
    QString id_;
    QString hostname_;

    double utilisation_;
};

#endif // MEMORYUTILISATIONEVENT_H