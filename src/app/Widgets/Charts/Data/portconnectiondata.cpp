#include "portconnectiondata.h"

std::atomic<int> PortConnectionData::port_connection_id(0);

/**
 * @brief PortConnectionData::PortConnectionData
 * @param exp_run_id
 * @param port_connection
 * @param parent
 */
PortConnectionData::PortConnectionData(quint32 exp_run_id, const AggServerResponse::PortConnection& port_connection, QObject *parent)
    : QObject(parent),
      experiment_run_id_(exp_run_id),
      from_port_graphml_id_(port_connection.from_port_graphml),
      to_port_graphml_id_(port_connection.to_port_graphml),
      type_(port_connection.type),
      port_connection_id_(port_connection_id++) {}


/**
 * @brief PortConnectionData::getID
 * @return
 */
int PortConnectionData::getID() const
{
    return port_connection_id_;
}


/**
 * @brief PortConnectionData::getFromPortID
 * @return
 */
const QString& PortConnectionData::getFromPortID() const
{
    return from_port_graphml_id_;
}


/**
 * @brief PortConnectionData::getToPortID
 * @return
 */
const QString& PortConnectionData::getToPortID() const
{
    return to_port_graphml_id_;
}


/**
 * @brief PortConnectionData::getPortConnectionType
 * @return
 */
AggServerResponse::PortConnection::ConnectionType PortConnectionData::getPortConnectionType() const
{
    return type_;
}

