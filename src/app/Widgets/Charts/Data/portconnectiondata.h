#ifndef PORTCONNECTIONDATA_H
#define PORTCONNECTIONDATA_H

#include "protomessagestructs.h"
#include <QObject>

// TODO - This class is not finished; will figure out what else is needed when Pulse moves to using these data classes
// Jackson: static ID counter has been removed as it is not currently used; replace when class is fleshed out
// static std::atomic<int> port_connection_id

class PortConnectionData : public QObject
{
    Q_OBJECT

public:
    PortConnectionData(quint32 exp_run_id, /*const QString& node_hostname,*/ const AggServerResponse::PortConnection& port_connection, QObject* parent = nullptr);

    const QString& getFromPortID() const;
    const QString& getToPortID() const;

    AggServerResponse::PortConnection::ConnectionType getPortConnectionType() const;

private:
    quint32 experiment_run_id_;

    QString from_port_graphml_id_;
    QString to_port_graphml_id_;

    AggServerResponse::PortConnection::ConnectionType type_;

};

#endif // PORTCONNECTIONDATA_H
