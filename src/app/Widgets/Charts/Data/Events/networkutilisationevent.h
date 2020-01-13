#ifndef NETWORKUTILISATIONEVENT_H
#define NETWORKUTILISATIONEVENT_H

#include "event.h"

class NetworkUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit NetworkUtilisationEvent(const QString& hostname,
                                    const QString& interface_mac_addr,
                                    qint64 delta_packets_sent,
                                    qint64 delta_packets_received,
                                    qint64 delta_bytes_sent,
                                    qint64 delta_bytes_received,
                                    qint64 time,
                                    QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const override;
    const QString& getSeriesID() const override;
    const QString& getID() const override;

    const QString& getHostname() const;
    const QString& getInterfaceMacAddress() const;

    qint64 getDeltaPacketsSent() const;
    qint64 getDeltaPacketsReceived() const;
    qint64 getDeltaBytesSent() const;
    qint64 getDeltaBytesReceived() const;

private:
    QString series_id_;
    QString id_;
    QString hostname_;
    QString interface_mac_address_;

    qint64 delta_packets_sent_;
    qint64 delta_packets_received_;
    qint64 delta_bytes_sent_;
    qint64 delta_bytes_received_;
};

#endif // NETWORKUTILISATIONEVENT_H
