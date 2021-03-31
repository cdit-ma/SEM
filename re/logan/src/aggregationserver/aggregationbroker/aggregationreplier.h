#ifndef AGGREGATIONREPLIER_H
#define AGGREGATIONREPLIER_H

#include "../databaseclient.h"
#include <zmq/protoreplier/protoreplier.hpp>

#include <proto/aggregationmessage/aggregationmessage.pb.h>

namespace re::logging::aggregation::broker {

/**
 * An AggregationReplier will listen for incoming Aggregation proto message requests and respond accordingly the
 * requested information from the DatabaseClient.
 * @exceptsafe none - calls to self, DatabaseClient and ProtoReplier offer no guarantees apart from where explicitly
 * stated. Direct throws are listed as they are present in the
 */
class AggregationReplier : public zmq::ProtoReplier {
public:
    /**
     * Use the provided DatabaseClient to communicate with the intended database, and bind incoming Aggregation proto
     * message requests to the relevant callback that will process and respond to it.
     * It is required that the DatabaseClient, which may be shared with other entities, remains valid for the entire
     * lifetime of the AggregationReplier.
     * @param db_client An initialised DatabaseClient
     */
    explicit AggregationReplier(std::shared_ptr<DatabaseClient> db_client);

    /**
     * Queries the database client in order to produce the required response to an ExperimentRun query
     * @param message The filtering details for the ExperimentRunRequest
     * @return The list of ExperimentRuns
     * @throws std::runtime_error: May fail to parse DB time strings
     */
    std::unique_ptr<AggServer::ExperimentRunResponse>
    ProcessExperimentRunRequest(const AggServer::ExperimentRunRequest& message);

    /**
     * Queries the database client in order to produce the required response to an ExperimentState query
     * @param message The filtering details for the ExperimentStateRequest
     * @return The structure of the requested experiment
     * @throws std::runtime_error - multiple entries received for a query of a single experiment
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::ExperimentStateResponse>
    ProcessExperimentStateRequest(const AggServer::ExperimentStateRequest& message);

    /**
     * Queries the database client in order to produce the required response to an PortLifecycle query
     * @param message The filtering details for the PortLifecycleRequest
     * @return The filtered PortLifecycle events
     * @throws std::runtime_error - failure to parse database stringed representation of enumerated types & times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::PortLifecycleResponse>
    ProcessPortLifecycleRequest(const AggServer::PortLifecycleRequest& message);

    /**
     * Queries the database client in order to produce the required response to an PortEvent query
     * @param message The filtering details for the PortRequest
     * @return The filtered Port events
     * @throws std::runtime_error - failure to parse database stringed representation of enumerated types & times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::PortEventResponse>
    ProcessPortEventRequest(const AggServer::PortEventRequest& message);

    /**
     * Queries the database client in order to produce the required response to an Workload query
     * @param message The filtering details for the WorkloadRequest
     * @return The filtered Workload events
     * @throws std::runtime_error - failure to parse database stringed representation of enumerated types & times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::WorkloadResponse>
    ProcessWorkloadEventRequest(const AggServer::WorkloadRequest& message);

    /**
     * Queries the database client in order to produce the required response to an Marker query
     * @param message The filtering details for the MarkerRequest
     * @return The filtered Marker events
     * @throws std::runtime_error - failure to parse database stringed representation of times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::MarkerResponse>
    ProcessMarkerRequest(const AggServer::MarkerRequest& message);

    /**
     * Queries the database client in order to produce the required response to an CPUUtilisation query
     * @param message The filtering details for the CPUUtilisationRequest
     * @return The filtered CPUUtilisation events
     * @throws std::runtime_error - failure to parse database stringed representation of times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::CPUUtilisationResponse>
    ProcessCPUUtilisationRequest(const AggServer::CPUUtilisationRequest& message);

    /**
     * Queries the database client in order to produce the required response to an MemoryUtilisation query
     * @param message The filtering details for the MemoryUtilisationRequest
     * @return The filtered MemoryUtilisation events
     * @throws std::runtime_error - failure to parse database stringed representation of times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::MemoryUtilisationResponse>
    ProcessMemoryUtilisationRequest(const AggServer::MemoryUtilisationRequest& message);

    /**
     * Queries the database client in order to produce the required response to an NetworkUtilisation query
     * @param message The filtering details for the NetworkUtilisationRequest
     * @return The filtered NetworkUtilisation events
     * @throws std::runtime_error - failure to parse database stringed representation of times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::NetworkUtilisationResponse>
    ProcessNetworkUtilisationRequest(const AggServer::NetworkUtilisationRequest& message);

    /**
     * Queries the database client in order to produce the required response to an NetworkUtilisation query
     * @param message The filtering details for the NetworkUtilisationRequest
     * @return The filtered NetworkUtilisation events
     * @throws std::runtime_error - failure to parse database stringed representation of times
     * @throws std::exception - rethrows any exceptions that occurred during internal function calls
     */
    std::unique_ptr<AggServer::GPUMetricResponse>
    ProcessGPUMetricRequest(const AggServer::GPUMetricRequest& message);

private:
    std::shared_ptr<DatabaseClient> database_;

    /**
     * Fill the required fields for this node, then recursively fill all of the constituent proto objects it contains
     * TODO: Stop leaking internal pqxx symbols through to a higher level API; should ideally be DB agnostic
     * @param node The protobuf node representation
     * @param node_values A row returned from a pqxx database query that is associated with a particular node
     * @param experiment_run_id The identifier for the experiment run this node is associated with
     */
    void FillNodeState(AggServer::Node& node, const pqxx::row& node_values, int experiment_run_id);

    /**
     * Fill the required fields for this container, then recursively fill all of the constituent proto objects it contains
     * TODO: Stop leaking internal pqxx symbols through to a higher level API; should ideally be DB agnostic
     * @param container The protobuf container representation
     * @param container_values A row returned from a pqxx database query that is associated with a particular container
     * @param experiment_run_id The identifier for the experiment run this container is associated with
     */
    void FillContainerState(AggServer::Container& container,
                            const pqxx::row& container_values,
                            int experiment_run_id);

    /**
     * Fill the required fields for the given protobuf interface representation
     * TODO: Stop leaking internal pqxx symbols through to a higher level API; should ideally be DB agnostic
     * @param interface The protobuf interface representation
     * @param interface_values A row returned from a pqxx database query that is associated with a particular interface
     * @param experiment_run_id The identifier for the experiment run this interface is associated with
     */
    void FillInterfaceState(AggServer::NetworkInterface& network_interface,
                            const pqxx::row& interface_values,
                            int experiment_run_id);

    /**
     * Fill the required fields for this component instance, then recursively fill all of the constituent proto objects it contains
     * TODO: Stop leaking internal pqxx symbols through to a higher level API; should ideally be DB agnostic
     * @param component_instance The protobuf component instance representation
     * @param component_instance_values A row returned from a pqxx database query that is associated with a particular component instance
     * @param experiment_run_id The identifier for the experiment run this component instance is associated with
     */
    void FillComponentInstanceState(AggServer::ComponentInstance& component_instance,
                                    const pqxx::row& component_instance_values,
                                    int experiment_run_id);

    /**
     * Fill the required fields for this worker instance proto representation
     * TODO: Stop leaking internal pqxx symbols through to a higher level API; should ideally be DB agnostic
     * @param worker_instance The protobuf worker instance representation
     * @param worker_instance_values A row returned from a pqxx database query that is associated with a particular worker instance
     * @param experiment_run_id The identifier for the experiment run this worker instance is associated with
     */
    void FillWorkerInstanceState(AggServer::WorkerInstance& worker_instance,
                                 const pqxx::row& worker_instance_values,
                                 int experiment_run_id);

    /**
     * Fill the required fields for this port proto representation
     * TODO: Stop leaking internal pqxx symbols through to a higher level API; should ideally be DB agnostic
     * @param port The protobuf port representation
     * @param port_values A row returned from a pqxx database query that is associated with a particular port
     */
    void FillPortState(AggServer::Port& port, const pqxx::row& port_values);

    /**
     * Set the required fields of the given PortConnection protobuf representation
     * @param port_connection The protobuf port representation
     * @param port_pair The [ to -> from ] mapping using stringed Port IDs
     * @param type The type of the connection (PubSub or ReqRep)
     */
    void FillPortConnectionState(AggServer::PortConnection& port_connection,
                                 const DatabaseClient::port_graphml_pair& port_pair,
                                 AggServer::PortConnection::ConnectionType type);

    /**
     * Register the various ProcessXYZRequest callbacks with the underlying ProtoReplier implementation
     */
    void RegisterCallbacks();
};

} // namespace re::logging::aggregation::broker

#endif // AGGREGATIONREPLIER_H