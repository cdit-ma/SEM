#ifndef PORTCONNECTIONDATA_H
#define PORTCONNECTIONDATA_H

#include "protomessagestructs.h"

class PortConnectionData {

public:
    PortConnectionData(const AggServerResponse::PortConnection& port_connection);

    int getID() const;

    const QString& getFromPortID() const;
    const QString& getToPortID() const;

    AggServerResponse::PortConnection::ConnectionType getPortConnectionType() const;

private:
    QString from_port_graphml_id_;
    QString to_port_graphml_id_;

    AggServerResponse::PortConnection::ConnectionType type_;

    int port_connection_id_ = -1;
    static std::atomic<int> port_connection_id;
};

#endif // PORTCONNECTIONDATA_H
