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
    explicit GPUMemoryUtilisationEvent(const QString& gpu_name,
                                       const QString& vendor,
                                       double utilisation,
                                       qint64 time,
                                       QObject* parent = nullptr);

    [[nodiscard]] QString toString(const QString& dateTimeFormat) const override;

    [[nodiscard]] const QString& getID() const override;
    [[nodiscard]] const QString& getGPUName() const;
    [[nodiscard]] const QString& getVendor() const;

    [[nodiscard]] double getValue() const;

private:
    QString id_;
    QString gpu_name_;
    QString vendor_;

    double utilisation_;
};

#endif // GPUMEMORYUTILISATIONEVENT_H