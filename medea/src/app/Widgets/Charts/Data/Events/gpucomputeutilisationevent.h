//
// Created by Cathlyn Aston on 5/3/21.
//

#ifndef GPUCOMPUTEUTILISATIONEVENT_H
#define GPUCOMPUTEUTILISATIONEVENT_H

#include "event.h"

class GPUComputeUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit GPUComputeUtilisationEvent(const QString& hostname,
                                        QString device_name,
                                        double utilisation,
                                        qint64 time,
                                        QObject* parent = nullptr);

    [[nodiscard]] QString toString(const QString& dateTimeFormat) const override;

    [[nodiscard]] const QString& getID() const override;
    [[nodiscard]] const QString& getHostname() const;

    [[nodiscard]] QString getDeviceName() const;
    [[nodiscard]] double getValue() const;

private:
    QString id_;
    QString hostname_;
    QString device_name_;

    double utilisation_;
};


#endif // GPUCOMPUTEUTILISATIONEVENT_H
