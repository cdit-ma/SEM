//
// Created by Cathlyn Aston on 5/3/21.
//

#ifndef GPUMEMORYUTILISATIONEVENT_H
#define GPUMEMORYUTILISATIONEVENT_H

#include "event.h"

class GPUMemoryUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit GPUMemoryUtilisationEvent(const QString& hostname,
                                       qint32 device_index,
                                       double utilisation,
                                       qint64 time,
                                       QObject* parent = nullptr);

    [[nodiscard]] QString toString(const QString& dateTimeFormat) const override;

    [[nodiscard]] const QString& getID() const override;
    [[nodiscard]] const QString& getHostname() const;

    [[nodiscard]] qint32 getDeviceIndex() const;
    [[nodiscard]] double getValue() const;

private:
    QString id_;
    QString hostname_;

    qint32 device_index_;
    double utilisation_;
};

#endif // GPUMEMORYUTILISATIONEVENT_H