#ifndef NETWORKUTILISATIONEVENT_H
#define NETWORKUTILISATIONEVENT_H

#include "event.h"

class NetworkUtilisationEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit NetworkUtilisationEvent(const QString& hostname,
                                    const QString& interface_mac_addr,
                                    qint64 packets_sent,
                                    qint64 packets_received,
                                    qint64 bytes_sent,
                                    qint64 bytes_received,
                                    qint64 time,
                                    QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const override;
    const QString& getSeriesID() const override;
    const QString& getID() const override;

    const QString& getHostname() const;
    const QString& getInterfaceMacAddress() const;

    qint64 getPacketsSent() const;
    qint64 getPacketsReceived() const;
    qint64 getBytesSent() const;
    qint64 getBytesReceived() const;

private:
    QString series_id_;
    QString id_;
    QString hostname_;
    QString interface_mac_address_;

    qint64 packets_sent_;
    qint64 packets_received_;
    qint64 bytes_sent_;
    qint64 bytes_received_;
};

#endif // NETWORKUTILISATIONEVENT_H
