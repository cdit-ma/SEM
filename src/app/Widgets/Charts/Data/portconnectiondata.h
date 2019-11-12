#ifndef PORTCONNECTIONDATA_H
#define PORTCONNECTIONDATA_H

#include "protomessagestructs.h"
#include <QObject>

// TODO - This class is not finished; will figure out what else is needed whe Pulse moves to using these data classes

class PortConnectionData : public QObject
{
    Q_OBJECT

public:
    PortConnectionData(quint32 exp_run_id, const AggServerResponse::PortConnection& port_connection, QObject* parent = nullptr);

    int getID() const;

    const QString& getFromPortID() const;
    const QString& getToPortID() const;

    AggServerResponse::PortConnection::ConnectionType getPortConnectionType() const;

private:
    quint32 experiment_run_id_;

    QString from_port_graphml_id_;
    QString to_port_graphml_id_;

    AggServerResponse::PortConnection::ConnectionType type_;

    int port_connection_id_ = -1;
    static std::atomic<int> port_connection_id;
};

#endif // PORTCONNECTIONDATA_H
