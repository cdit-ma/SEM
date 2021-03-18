//
// Created by Cathlyn Aston on 5/3/21.
//

#ifndef GPUTEMPERATUREEVENT_H
#define GPUTEMPERATUREEVENT_H

#include "event.h"

class GPUTemperatureEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit GPUTemperatureEvent(const QString& hostname,
                                 qint32 device_index,
                                 double temperature,
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
    double temperature_;
};

#endif // GPUTEMPERATUREEVENT_H
