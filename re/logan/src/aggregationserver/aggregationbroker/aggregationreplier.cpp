#include "aggregationreplier.h"

#include <google/protobuf/util/time_util.h>

#include "../prototimerange.hpp"
#include <memory>
#include <utility>

using google::protobuf::util::TimeUtil;

namespace re::logging::aggregation::broker {

/**
 * Internal details namespace for the Broker class, not intended for external use (move elsewhere and refactor as
 * as necessary if functionality is needed)
 */
namespace detail {
std::string StringToPSQLTimestamp(const std::string& str)
{
    return DatabaseClient::StringToPSQLTimestamp(str);
}

/**
 * Class that enforces pairing rules for SQL conditional filtering clauses
 * TODO: Enforce this pairing through an actual object representation of each "pair" (include condition?)
 */
class ConditionPairList {
private:
    std::vector<std::string> cols_;
    std::vector<std::string> vals_;

public:
    /**
     * Associate the given table column name and value to be used in a conditional comparison at some point
     * @param col_name The name of the column for a particular table
     * @param val The value to be associated with the column
     */
    void addPair(const std::string& col_name, const std::string& val)
    {
        cols_.emplace_back(col_name);
        vals_.emplace_back(val);
    }
    [[nodiscard]] const std::vector<std::string>& getColumns() const { return cols_; }
    [[nodiscard]] const std::vector<std::string>& getValues() const { return vals_; }
};

/**
 * Convenience builder class to make the construction of ConditionPairLists more ergonomic
 */
class ConditionPairBuilder {
private:
    ConditionPairList pair_list_;
    ConditionPairBuilder() = default;

public:
    /**
     * Start building a ConditionPairList
     * @return A ConditionPairBuilder
     */
    static ConditionPairBuilder createConditionPairs() { return ConditionPairBuilder(); }

    /**
     * Associates one or more values with the provided column name (condition not specified)
     * @param col_name The column name within the table to be queried
     * @param values One or more stringed representations of the values to be checked against
     * @return A reference to the ConditionPairBuilder so that the API usage continues to flow nicely
     */
    ConditionPairBuilder& add(const std::string& col_name, const std::vector<std::string>& values)
    {
        for(const auto& val : values) {
            pair_list_.addPair(col_name, val);
        }
        return *this;
    }
    /**
     * Associates one or more values with the provided column name (condition not specified)
     * @param col_name The column name within the table to be queried
     * @param values One or more Protobuf representations of the values to be checked against
     * @return A reference to the ConditionPairBuilder so that the API usage continues to flow nicely
     */
    ConditionPairBuilder&
    add(const std::string& col_name, const google::protobuf::RepeatedPtrField<std::string>& values)
    {
        for(const auto& val : values) {
            pair_list_.addPair(col_name, val);
        }
        return *this;
    }
    /**
     * Finalise the building process by obtaining the ConditionPairList
     * @return The constructed ConditionPairList
     */
    ConditionPairList finish() { return pair_list_; }
};
} // namespace detail
using namespace detail;

// Still in use as some code is yet to move out of the legacy AggServer namespace
using namespace AggServer;

AggregationReplier::AggregationReplier(std::shared_ptr<DatabaseClient> database) :
    database_(std::move(database))
{
    RegisterCallbacks();
}

void AggregationReplier::RegisterCallbacks()
{
    RegisterProtoCallback<ExperimentRunRequest, ExperimentRunResponse>(
        "GetExperimentRuns",
        std::bind(&AggregationReplier::ProcessExperimentRunRequest, this, std::placeholders::_1));
    RegisterProtoCallback<ExperimentStateRequest, ExperimentStateResponse>(
        "GetExperimentState",
        std::bind(&AggregationReplier::ProcessExperimentStateRequest, this, std::placeholders::_1));
    RegisterProtoCallback<PortLifecycleRequest, PortLifecycleResponse>(
        "GetPortLifecycle",
        std::bind(&AggregationReplier::ProcessPortLifecycleRequest, this, std::placeholders::_1));
    RegisterProtoCallback<PortEventRequest, PortEventResponse>(
        "GetPortEvents",
        std::bind(&AggregationReplier::ProcessPortEventRequest, this, std::placeholders::_1));
    RegisterProtoCallback<WorkloadRequest, WorkloadResponse>(
        "GetWorkload",
        std::bind(&AggregationReplier::ProcessWorkloadEventRequest, this, std::placeholders::_1));
    RegisterProtoCallback<MarkerRequest, MarkerResponse>(
        "GetMarkers",
        std::bind(&AggregationReplier::ProcessMarkerRequest, this, std::placeholders::_1));
    RegisterProtoCallback<CPUUtilisationRequest, CPUUtilisationResponse>(
        "GetCPUUtilisation",
        std::bind(&AggregationReplier::ProcessCPUUtilisationRequest, this, std::placeholders::_1));
    RegisterProtoCallback<MemoryUtilisationRequest, MemoryUtilisationResponse>(
        "GetMemoryUtilisation", std::bind(&AggregationReplier::ProcessMemoryUtilisationRequest,
                                          this, std::placeholders::_1));
    RegisterProtoCallback<NetworkUtilisationRequest, NetworkUtilisationResponse>(
        "GetNetworkUtilisation", std::bind(&AggregationReplier::ProcessNetworkUtilisationRequest,
                                           this, std::placeholders::_1));
    RegisterProtoCallback<GPUMetricRequest, GPUMetricResponse>(
        "GetGPUMetrics", std::bind(&AggregationReplier::ProcessGPUMetricRequest,
                                           this, std::placeholders::_1));
}

std::unique_ptr<ExperimentRunResponse>
AggregationReplier::ProcessExperimentRunRequest(const ExperimentRunRequest& message)
{
    auto response = std::make_unique<ExperimentRunResponse>();

    // Generate a list of Experiment name + ID pairs
    const std::string& exp_name = message.experiment_name();
    std::vector<std::pair<std::string, int>> exp_name_id_pairs;
    if(exp_name.empty()) {
        // If no name is provided ask the database for the list of all names and ExperimentIDs
        const auto& results = database_->GetValues("Experiment", {"Name", "ExperimentID"});
        for(const auto& row : results) {
            const auto& name = row.at("Name").as<std::string>();
            const auto& id = row.at("ExperimentID").as<int>();
            exp_name_id_pairs.emplace_back(std::make_pair(name, id));
        }
    } else {
        // Otherwise, get the names and IDs of any experiments who's name contains the provided exp_name string
        const auto& results = database_->GetValues("Experiment", {"Name", "ExperimentID"},
                                                   "Name LIKE "
                                                       + database_->quote("%" + exp_name + "%"));
        for(const auto& row : results) {
            const auto& name = row.at("Name").as<std::string>();
            const auto& id = row.at("ExperimentID").as<int>();
            exp_name_id_pairs.emplace_back(std::make_pair(name, id));
        }
    }

    // Use the name + ID pairs to determine which Experiments we need to make Protobuf representations of
    for(const auto& pair : exp_name_id_pairs) {
        const auto& name = pair.first;
        const auto& id = pair.second;
        auto exp_info = response->add_experiments();
        exp_info->set_name(name);

        // Get the details of each identified experiment
        const auto& results = database_->GetValues(
            "ExperimentRun",
            {"ExperimentRunID", "JobNum", StringToPSQLTimestamp("StartTime") + " AS StartTime",
             StringToPSQLTimestamp("EndTime") + " AS EndTime"},
            "ExperimentID = " + std::to_string(id));

        for(const auto& row : results) {
            auto run = exp_info->add_runs();
            run->set_experiment_run_id(row.at("ExperimentRunID").as<int>());
            run->set_job_num(row.at("JobNum").as<int>());
            auto&& start_time_str = row.at("StartTime").as<std::string>();
            // REVIEW(Mitch): Wrap this function in something that uses result based error reporting?
            // mechanisms.
            bool success = TimeUtil::FromString(start_time_str, run->mutable_start_time());
            if(!success) {
                throw std::runtime_error("Unable to parse ExperimentRun.StartTime from in-database "
                                         "string representation");
            }
            try {
                auto&& end_time_str = row.at("EndTime").as<std::optional<std::string>>();
                // If the end time has not yet been set then the experiment hasn't finished,
                // leave proto field default initialised
                if(end_time_str.has_value()) {
                    // REVIEW(Mitch): Handle error case
                    TimeUtil::FromString(*end_time_str, run->mutable_end_time());
                }
            } catch(const std::exception& e) {
                std::cerr << "An exception occurred while querying experiment run end time"
                          << e.what() << "\n";
            }
        }
    }

    return response;
}

std::unique_ptr<ExperimentStateResponse>
AggregationReplier::ProcessExperimentStateRequest(const ExperimentStateRequest& message)
{
    auto response = std::make_unique<ExperimentStateResponse>();

    const int run_id = message.experiment_run_id();

    try {
        const auto& run_results =
            database_->GetValues("ExperimentRun",
                                 {"JobNum", StringToPSQLTimestamp("StartTime") + " AS StartTime",
                                  StringToPSQLTimestamp("EndTime") + " AS EndTime",
                                  StringToPSQLTimestamp("LastUpdated") + " AS LastUpdated"},
                                 "ExperimentRunID = " + std::to_string(run_id));

        // 0 results indicates experiment run, more than one means conflicting experiment runs exist
        if(run_results.size() == 1) {
            try {
                const auto& last_updated = run_results.at(0).at("LastUpdated").as<std::string>();
                TimeUtil::FromString(last_updated, response->mutable_last_updated());
            } catch(const std::exception& e) {
                std::cerr << "Unable to retrieve the last time experiment run was updated:\n"
                          << e.what() << std::endl;
            }
            try {
                const auto& end_time =
                    run_results.at(0).at("EndTime").as<std::optional<std::string>>();
                // If no end time exists leave the proto field default initialised
                if(end_time) {
                    TimeUtil::FromString(*end_time, response->mutable_end_time());
                }
            } catch(const std::exception& e) {
                std::cerr << "Unable to retrieve the shutdown time of the experiment run:\n"
                          << e.what() << std::endl;
            }
        } else {
            throw std::runtime_error("Expected exactly one result row when querying DB for "
                                     "experiment state, received "
                                     + std::to_string(run_results.size()));
        }

        const auto& node_results = database_->GetValues(
            "Node", {"NodeID", "Hostname", "IP"}, "ExperimentRunID = " + std::to_string(run_id));
        for(const auto& row : node_results) {
            FillNodeState(*response->add_nodes(), row, run_id);
        }

        const auto& component_results = database_->GetValues(
            "Component", {"Name"}, "ExperimentRunID = " + std::to_string(run_id));
        for(const auto& row : component_results) {
            auto new_component = response->add_components();
            new_component->set_name(row.at("Name").as<std::string>());
        }

        const auto& worker_results =
            database_->GetValues("Worker", {"Name"}, "ExperimentRunID = " + std::to_string(run_id));
        for(const auto& row : worker_results) {
            auto new_component = response->add_workers();
            new_component->set_name(row.at("Name").as<std::string>());
        }

        for(const auto& pair : database_->GetPubSubConnectionIDs(run_id)) {
            FillPortConnectionState(*response->add_port_connections(), pair,
                                    PortConnection::PUBSUB);
        }
        for(const auto& pair : database_->GetReqRepConnectionIDs(run_id)) {
            FillPortConnectionState(*response->add_port_connections(), pair,
                                    PortConnection::REQREP);
        }
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while populating ExperimentStateResponse with "
                     "ExperimentRunID="
                  << run_id << ": " << e.what() << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<PortLifecycleResponse>
AggregationReplier::ProcessPortLifecycleRequest(const PortLifecycleRequest& message)
{
    auto response = std::make_unique<PortLifecycleResponse>();

    std::string start, end;

    // Start time defaults to 0 if not specified
    if(message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {
        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if(message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    const auto conditions = ConditionPairBuilder::createConditionPairs()
                                .add("Port.Path", message.port_paths())
                                .add("Port.GraphmlID", message.port_ids())
                                .add("ComponentInstance.Path", message.component_instance_paths())
                                .add("ComponentInstance.GraphmlID",
                                     message.component_instance_ids())
                                .add("Component.Name", message.component_names())
                                .finish();

    try {
        const pqxx::result res = database_->GetPortLifecycleEventInfo(message.experiment_run_id(),
                                                                      start, end,
                                                                      conditions.getColumns(),
                                                                      conditions.getValues());

        for(const auto& row : res) {
            auto event = response->add_events();

            // Build Event
            // REVIEW(Mitchell): Break both this and Build Port block into funcs
            auto&& type_str = row["Type"].as<std::string>();
            LifecycleType lifecycle_type;
            bool did_parse_lifecycle_type = LifecycleType_Parse(type_str, &lifecycle_type);
            if(!did_parse_lifecycle_type) {
                throw std::runtime_error("Failed to parse LifecycleType field from string: "
                                         + type_str);
            }
            event->set_type(lifecycle_type);
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if(!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }

            // Build Port
            auto port = event->mutable_port();
            port->set_name(row["PortName"].as<std::string>());
            port->set_path(row["PortPath"].as<std::string>());
            Port::Kind kind;
            bool did_parse_lifecycle = Port::Kind_Parse(row["PortKind"].as<std::string>(), &kind);
            if(!did_parse_lifecycle) {
                throw std::runtime_error("Failed to parse Port's Kind field from string: "
                                         + row["PortKind"].as<std::string>());
            }
            port->set_kind(kind);
            port->set_middleware(row["Middleware"].as<std::string>());
            port->set_graphml_id(row["PortGraphmlID"].as<std::string>());
        }

    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying PortLifecycleEvents:" << ex.what()
                  << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<PortEventResponse>
AggregationReplier::ProcessPortEventRequest(const PortEventRequest& message)
{
    auto response = std::make_unique<PortEventResponse>();

    std::string start, end;

    // REVIEW(Mitchell): This pattern seems to be duplicated in a few places.
    //   auto get_start_end_time(const RepeatedPtrField<Timestamp>& times) ->
    //   std::pair<std::string, std::optional<std::string>
    // Start time defaults to 0 if not specified
    if(message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {
        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if(message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    const auto conditions = ConditionPairBuilder::createConditionPairs()
                                .add("Port.Path", message.port_paths())
                                .add("Port.GraphmlID", message.port_ids())
                                .add("ComponentInstance.Path", message.component_instance_paths())
                                .add("ComponentInstance.GraphmlID",
                                     message.component_instance_ids())
                                .add("Component.Name", message.component_names())
                                .finish();

    try {
        const pqxx::result res = database_->GetPortEventInfo(message.experiment_run_id(), start,
                                                             end, conditions.getColumns(),
                                                             conditions.getValues());

        for(const auto& row : res) {
            auto event = response->add_events();

            // Build Event
            auto&& db_type_str = row["Type"].as<std::string>();
            PortEvent::PortEventType event_type;
            bool did_parse_type = PortEvent_PortEventType_Parse(db_type_str, &event_type);
            if(!did_parse_type) {
                throw std::runtime_error("Failed to parse PortEventType from string: "
                                         + db_type_str);
            }
            event->set_type(event_type);
            event->set_message(row["Message"].as<std::string>());
            event->set_sequence_num(row["SequenceNum"].as<int>());
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse_event_time = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if(!did_parse_event_time) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }

            // Build Port
            auto port = event->mutable_port();
            port->set_name(row["PortName"].as<std::string>());
            port->set_path(row["PortPath"].as<std::string>());
            Port::Kind kind;
            bool did_parse_port_kind = Port::Kind_Parse(row["PortKind"].as<std::string>(), &kind);
            if(!did_parse_port_kind) {
                throw std::runtime_error("Failed to parse Port's Kind field from string: "
                                         + row["PortKind"].as<std::string>());
            }
            port->set_kind(kind);
            port->set_middleware(row["Middleware"].as<std::string>());
            port->set_graphml_id(row["PortGraphmlID"].as<std::string>());
        }

    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying PortEvents:" << ex.what() << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<WorkloadResponse>
AggregationReplier::ProcessWorkloadEventRequest(const WorkloadRequest& message)
{
    auto response = std::make_unique<WorkloadResponse>();

    std::string start, end;

    // Start time defaults to 0 if not specified
    if(message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {
        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if(message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    const auto conditions = ConditionPairBuilder::createConditionPairs()
                                .add("WorkerInstance.Path", message.worker_paths())
                                .add("Worker.GraphmlID", message.worker_instance_ids())
                                .add("ComponentInstance.Path", message.component_instance_paths())
                                .add("ComponentInstance.GraphmlID",
                                     message.component_instance_ids())
                                .add("Component.Name", message.component_names())
                                .finish();

    try {
        const pqxx::result res = database_->GetWorkloadEventInfo(message.experiment_run_id(), start,
                                                                 end, conditions.getColumns(),
                                                                 conditions.getValues());

        for(const auto& row : res) {
            auto event = response->add_events();

            // Build Event
            WorkloadEvent::WorkloadEventType type;
            auto&& type_string = row["Type"].as<std::string>();
            bool did_parse_workloadtype = WorkloadEvent::WorkloadEventType_Parse(type_string,
                                                                                 &type);
            if(!did_parse_workloadtype) {
                // Workaround for string mismatch due to Windows being unable to handle ERROR as a
                // name (LOG-94)
                if(type_string == "ERROR") {
                    type = WorkloadEvent::ERROR_EVENT;
                } else {
                    throw std::runtime_error("Unable to parse WorkloadEventType from string: "
                                             + type_string);
                }
            }
            event->set_type(type);
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if(!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }
            event->set_function_name(row["FunctionName"].as<std::string>());
            event->set_args(row["Arguments"].as<std::string>());

            // Build WorkerInstance
            auto worker_inst = event->mutable_worker_inst();
            worker_inst->set_name(row["WorkerInstanceName"].as<std::string>());
            worker_inst->set_path(row["WorkerInstancePath"].as<std::string>());
            worker_inst->set_graphml_id(row["WorkerInstanceGraphmlID"].as<std::string>());
        }

    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying WorkloadEvents:" << ex.what()
                  << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<MarkerResponse>
AggregationReplier::ProcessMarkerRequest(const MarkerRequest& message)
{
    auto response = std::make_unique<MarkerResponse>();

    std::string start, end;

    // REVIEW(Mitchell): Why is this one different to the rest?
    //  This is a bug as far as I can tell.
    // Start time defaults to 0 if not specified
    if(message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {
        start = "";
    }

    // End time defaults to 0 if not specified
    if(message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = "";
    }

    // Get filter conditions
    const auto conditions = ConditionPairBuilder::createConditionPairs()
                                .add("WorkerInstance.Path", message.worker_instance_paths())
                                .add("Worker.GraphmlID", message.worker_instance_ids())
                                .add("ComponentInstance.Path", message.component_instance_paths())
                                .add("ComponentInstance.GraphmlID",
                                     message.component_instance_ids())
                                .add("Component.Name", message.component_names())
                                .finish();

    try {
        const pqxx::result res = database_->GetMarkerInfo(message.experiment_run_id(), start, end,
                                                          conditions.getColumns(),
                                                          conditions.getValues());

        std::unordered_map<std::string, MarkerNameSet*> name_set_map;
        std::unordered_map<std::string, MarkerIDSet*> id_set_map;

        for(const auto& row : res) {
            // Get label and create new name_set if one doesn't already exist
            const std::string& label = row["Label"].as<std::string>();
            MarkerNameSet* name_set = nullptr;
            try {
                name_set = name_set_map.at(label);
            } catch(const std::out_of_range& oor_exception) {
                name_set = response->add_marker_name_sets();
                name_set->set_name(label);
                name_set_map.emplace(label, name_set);
            }

            // Get id and create new id_set if one doesn't already exist for the given name+id combo
            int id = row["WorkloadID"].as<int>();
            std::string id_str = label + '_' + std::to_string(id);
            MarkerIDSet* id_set = nullptr;
            try {
                id_set = id_set_map.at(id_str);
            } catch(const std::out_of_range& oor_exception) {
                id_set = name_set->add_marker_id_set();
                id_set->set_id(id);
                id_set_map.emplace(id_str, id_set);
            }

            // Build MarkerEvent
            MarkerEvent* event = id_set->add_events();
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_timestamp());
            if(!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }

            // Build ComponentInstance
            auto comp_inst = event->mutable_component_instance();
            comp_inst->set_name(row["ComponentInstanceName"].as<std::string>());
            comp_inst->set_path(row["ComponentInstancePath"].as<std::string>());
            comp_inst->set_graphml_id(row["ComponentInstanceGraphmlID"].as<std::string>());
            comp_inst->set_type(row["ComponentName"].as<std::string>());

            // Build WorkerInstance
            auto worker_inst = comp_inst->add_worker_instances();
            worker_inst->set_name(row["WorkerInstanceName"].as<std::string>());
            worker_inst->set_path(row["WorkerInstancePath"].as<std::string>());
            worker_inst->set_graphml_id(row["WorkerInstanceGraphmlID"].as<std::string>());
            worker_inst->set_type(row["WorkerName"].as<std::string>());
        }

    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying MarkerEvents:" << ex.what() << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<CPUUtilisationResponse>
AggregationReplier::ProcessCPUUtilisationRequest(const CPUUtilisationRequest& message)
{
    auto response = std::make_unique<CPUUtilisationResponse>();

    std::string start, end;

    // Start time defaults to 0 if not specified
    if(message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {
        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if(message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    std::vector<std::string> condition_cols;
    std::vector<std::string> condition_vals;
    for(const auto& id : message.node_ids()) {
        condition_cols.emplace_back("Node.GraphmlID");
        condition_vals.emplace_back(id);
    }
    for(const auto& hostname : message.node_hostnames()) {
        condition_cols.emplace_back("Node.HostName");
        condition_vals.emplace_back(hostname);
    }

    try {
        // NOTE: assumes that the database provides results sorted by hostname!!
        std::string current_hostname;
        CPUUtilisationNode* current_node = nullptr;
        const pqxx::result res = database_->GetCPUUtilInfo(message.experiment_run_id(), start, end,
                                                           condition_cols, condition_vals);

        for(const auto& row : res) {
            // Check if we need to create a new Node due to encountering a new hostname
            auto&& hostname = row["NodeHostname"].as<std::string>();
            if(current_hostname != hostname) {
                current_hostname = hostname;
                current_node = response->add_nodes();
                current_node->mutable_node_info()->set_hostname(hostname);
                current_node->mutable_node_info()->set_ip(row["NodeIP"].as<std::string>());
            }
            auto event = current_node->add_events();

            // Build Event
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if(!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }
            event->set_cpu_utilisation(row["CPUUtilisation"].as<double>());
        }
    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying CPUUtilisationEvents:" << ex.what()
                  << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<MemoryUtilisationResponse>
AggregationReplier::ProcessMemoryUtilisationRequest(const MemoryUtilisationRequest& message)
{
    auto response = std::make_unique<MemoryUtilisationResponse>();

    std::string start, end;

    // Start time defaults to 0 if not specified
    if(message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {
        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if(message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    std::vector<std::string> condition_cols;
    std::vector<std::string> condition_vals;
    for(const auto& id : message.node_ids()) {
        condition_cols.emplace_back("Node.GraphmlID");
        condition_vals.emplace_back(id);
    }
    for(const auto& hostname : message.node_hostnames()) {
        condition_cols.emplace_back("Node.HostName");
        condition_vals.emplace_back(hostname);
    }

    try {
        // NOTE: assumes that the database provides results sorted by hostname!!
        std::string current_hostname;
        MemoryUtilisationNode* current_node = nullptr;
        const pqxx::result res = database_->GetMemUtilInfo(message.experiment_run_id(), start, end,
                                                           condition_cols, condition_vals);

        for(const auto& row : res) {
            // Check if we need to create a new Node due to encoutnering a new hostname
            auto&& hostname = row["NodeHostname"].as<std::string>();
            if(current_hostname != hostname) {
                current_hostname = hostname;
                current_node = response->add_nodes();
                current_node->mutable_node_info()->set_hostname(hostname);
                current_node->mutable_node_info()->set_ip(row["NodeIP"].as<std::string>());
            }
            auto event = current_node->add_events();

            // Build Event
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if(!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }
            event->set_memory_utilisation(row["PhysMemUtilisation"].as<double>());
        }
    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying MemoryUtilisationEvents:" << ex.what()
                  << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<NetworkUtilisationResponse>
AggregationReplier::ProcessNetworkUtilisationRequest(const NetworkUtilisationRequest& message)
{
    auto response = std::make_unique<NetworkUtilisationResponse>();

    auto time_interval = re::types::proto::FromProto(message.time_interval());

    // Get filter conditions
    std::vector<std::string> condition_cols;
    std::vector<std::string> condition_vals;
    for(const auto& id : message.node_ids()) {
        condition_cols.emplace_back("Node.GraphmlID");
        condition_vals.emplace_back(id);
    }
    for(const auto& hostname : message.node_hostnames()) {
        condition_cols.emplace_back("Node.HostName");
        condition_vals.emplace_back(hostname);
    }

    try {
        // NOTE: assumes that the database provides results sorted by hostname!!
        std::string current_hostname, current_interface_mac;
        NodeNetworkEvents* current_node_event_group = nullptr;
        InterfaceNetworkEvents* current_interface_event_group = nullptr;

        const pqxx::result res = database_->GetNetworkUtilInfo(message.experiment_run_id(),
                                                               time_interval, condition_cols,
                                                               condition_vals);

        for(const auto& row : res) {
            // Check if we need to create a new Node grouping due to encountering a new hostname
            auto&& hostname = row["NodeHostname"].as<std::string>();
            if(current_hostname != hostname) {
                current_hostname = hostname;
                current_node_event_group = response->add_node_network_events();
                current_node_event_group->mutable_node_info()->set_hostname(hostname);
                current_node_event_group->mutable_node_info()->set_ip(
                    row["NodeIP"].as<std::string>());
            }

            // Check if we need to create a new Interface grouping due to encountering a new MAC
            auto&& interface_mac = row["MAC"].as<std::string>();
            if(current_interface_mac != interface_mac) {
                current_interface_mac = interface_mac;
                current_interface_event_group = current_node_event_group->add_events();
                current_interface_event_group->set_interface_mac_addr(interface_mac);
            }
            auto interface_event = current_interface_event_group->add_events();

            // Build Event
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, interface_event->mutable_time());
            if(!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }
            interface_event->set_bytes_sent(row["BytesSent"].as<uint64_t>());
            interface_event->set_bytes_received(row["BytesReceived"].as<uint64_t>());
        }
    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying NetworkUtilisationEvents:" << ex.what()
                  << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<GPUMetricResponse>
AggregationReplier::ProcessGPUMetricRequest(const GPUMetricRequest& message)
{
    auto response = std::make_unique<GPUMetricResponse>();

    auto time_interval = re::types::proto::FromProto(message.time_interval());

    // Get filter conditions
    std::vector<std::string> condition_cols;
    std::vector<std::string> condition_vals;
    for(const auto& id : message.node_ids()) {
        condition_cols.emplace_back("Node.GraphmlID");
        condition_vals.emplace_back(id);
    }
    for(const auto& hostname : message.node_hostnames()) {
        condition_cols.emplace_back("Node.HostName");
        condition_vals.emplace_back(hostname);
    }

    try {
        // NOTE: assumes that the database provides results sorted by hostname!!
        std::string current_hostname, current_device_name;
        NodeGPUMetrics* current_node_group = nullptr;
        GPUDeviceMetrics* current_gpu_group = nullptr;

        const pqxx::result res = database_->GetGPUMetricInfo(message.experiment_run_id(),
                                                               time_interval, condition_cols,
                                                               condition_vals);

        for(const auto& row : res) {
            // Check if we need to create a new Node grouping due to encountering a new hostname
            auto&& hostname = row["NodeHostname"].as<std::string>();
            if(current_hostname != hostname) {
                current_hostname = hostname;
                current_node_group = response->add_node_gpu_metrics();
                current_node_group->mutable_node_info()->set_hostname(hostname);
                current_node_group->mutable_node_info()->set_ip(
                    row["NodeIP"].as<std::string>());
            }

            // Check if we need to create a new Interface grouping due to encountering a new MAC
            auto&& gpu_name = row["GPUName"].as<std::string>();
            if(current_device_name != gpu_name) {
                current_device_name = gpu_name;
                current_gpu_group = current_node_group->add_per_device_metrics();
                current_gpu_group->set_gpu_device_name(gpu_name);
            }
            auto gpu_metric_sample = current_gpu_group->add_samples();

            // Build Event
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, gpu_metric_sample->mutable_time());
            if(!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "
                                         + timestamp_str);
            }
            gpu_metric_sample->set_gpu_utilisation(row["GPUUtilisation"].as<uint64_t>());
            gpu_metric_sample->set_mem_utilisation_mib(row["MemoryUtilisation"].as<uint64_t>());
            gpu_metric_sample->set_mem_utilisation_mib(row["Temperature"].as<uint64_t>());
        }
    } catch(const std::exception& ex) {
        std::cerr << "An exception occurred while querying GPUMetricEvents:" << ex.what()
                  << std::endl;
        throw;
    }

    return response;
}

void AggregationReplier::FillNodeState(Node& node,
                                       const pqxx::row& node_values,
                                       int experiment_run_id)
{
    auto node_id = node_values.at("NodeID").as<std::string>();
    node.set_hostname(node_values.at("Hostname").as<std::string>());
    node.set_ip(node_values.at("IP").as<std::string>());

    try {
        const auto& container_results = database_->GetValues("Container", {"ContainerID", "Name", "Type"},
                                                   "NodeID = " + node_id);
        for(const auto& row : container_results) {
            FillContainerState(*node.add_containers(), row, experiment_run_id);
        }

        const auto& interface_results = database_->GetValues(
            "Hardware.Interface",
            {"InterfaceID", "Name", "Type", "Description", "IPv4", "IPv6", "MAC", "Speed"},
            "NodeID = " + node_id);

        for(const auto& row : interface_results) {
            FillInterfaceState(*node.add_interfaces(), row, experiment_run_id);
        }

    } catch(const std::exception& e) {
        std::cerr << "An exception occured while populating ExperimentStateResponse Node with "
                     "NodeID="
                  << node_id << std::endl;
        throw;
    }
}

void AggregationReplier::FillContainerState(Container& container,
                                            const pqxx::row& container_values,
                                            int experiment_run_id)
{
    auto container_id = container_values.at("ContainerID").as<std::string>();
    container.set_name(container_values.at("Name").as<std::string>());
    Container::ContainerType type;
    Container::ContainerType_Parse(container_values.at("Type").as<std::string>(), &type);
    container.set_type(type);

    try {
        const auto& results = database_->GetValues("ComponentInstance",
                                                   {"ComponentInstanceID", "Name", "ContainerID",
                                                    "ComponentID", "Path", "GraphmlID"},
                                                   "ContainerID = " + container_id);
        for(const auto& row : results) {
            FillComponentInstanceState(*container.add_component_instances(), row,
                                       experiment_run_id);
        }
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while populating ExperimentStateResponse Container "
                     "with ContainerID=" << container_id << std::endl;
        throw;
    }
}

void AggregationReplier::FillInterfaceState(NetworkInterface& network_interface,
                                            const pqxx::row& interface_values,
                                            int experiment_run_id)
{
    int network_interface_id = -1;
    try {
        network_interface_id = interface_values.at("InterfaceID").as<int>();
        network_interface.set_name(interface_values.at("Name").as<std::string>());
        network_interface.set_type(interface_values.at("Type").as<std::string>());
        network_interface.set_description(interface_values.at("Description").as<std::string>());
        network_interface.set_ipv4(interface_values.at("IPv4").as<std::string>());
        network_interface.set_ipv6(interface_values.at("IPv6").as<std::string>());
        network_interface.set_mac_address(interface_values.at("MAC").as<std::string>());
        network_interface.set_speed(interface_values.at("Speed").as<int64_t>());
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while populating ExperimentStateResponse "
                     "NetworkInterface with ID="
                  << network_interface_id << std::endl;
        throw;
    }
}

void AggregationReplier::FillComponentInstanceState(ComponentInstance& component_instance,
                                                    const pqxx::row& component_instance_values,
                                                    int experiment_run_id)
{
    auto component_instance_id =
        component_instance_values.at("ComponentInstanceID").as<std::string>();
    component_instance.set_name(component_instance_values.at("Name").as<std::string>());
    component_instance.set_path(component_instance_values.at("Path").as<std::string>());
    component_instance.set_graphml_id(component_instance_values.at("GraphmlID").as<std::string>());

    const std::string& component_id = component_instance_values.at("ComponentID").as<std::string>();

    try {
        const auto& component_results =
            database_->GetValues("Component", {"Name"},
                                 "ComponentID = " + component_id + " AND ExperimentRunID = "
                                     + std::to_string(experiment_run_id));
        for(const auto& row : component_results) {
            component_instance.set_type(row.at("Name").as<std::string>());
        }

        const auto& port_results =
            database_->GetValues("Port", {"Name", "Kind", "Path", "GraphmlID", "Middleware"},
                                 "ComponentInstanceID = " + component_instance_id);
        for(const auto& row : port_results) {
            FillPortState(*component_instance.add_ports(), row);
        }

        const auto& worker_instance_results =
            database_->GetValues("WorkerInstance", {"Name", "WorkerID", "Path", "GraphmlID"},
                                 "ComponentInstanceID = " + component_instance_id);
        for(const auto& row : worker_instance_results) {
            FillWorkerInstanceState(*component_instance.add_worker_instances(), row,
                                    experiment_run_id);
        }
    } catch(const std::exception& e) {
        std::cerr << "An exception occurred while populating ExperimentStateResponse "
                     "ComponentInstance with ID="
                  << component_instance_id << std::endl;
        throw;
    }
}

void AggregationReplier::FillWorkerInstanceState(WorkerInstance& worker_instance,
                                                 const pqxx::row& worker_instance_values,
                                                 int experiment_run_id)
{
    worker_instance.set_name(worker_instance_values.at("Name").as<std::string>());
    worker_instance.set_path(worker_instance_values.at("Path").as<std::string>());
    worker_instance.set_graphml_id(worker_instance_values.at("GraphmlID").as<std::string>());
    const std::string& worker_id = worker_instance_values.at("WorkerID").as<std::string>();

    // Go to the definition of the worker to get its type
    const auto& worker_results = database_->GetValues(
        "Worker", {"Name"},
        "WorkerID = " + worker_id + " AND ExperimentRunID = " + std::to_string(experiment_run_id));
    for(const auto& row : worker_results) {
        worker_instance.set_type(row.at("Name").as<std::string>());
    }
}

void AggregationReplier::FillPortState(Port& port, const pqxx::row& port_values)
{
    port.set_name(port_values.at("Name").as<std::string>());
    Port::Kind kind;
    Port::Kind_Parse(port_values.at("Kind").as<std::string>(), &kind);
    port.set_kind(kind);
    port.set_path(port_values.at("Path").as<std::string>());
    port.set_middleware(port_values.at("Middleware").as<std::string>());
    port.set_graphml_id(port_values.at("GraphmlID").as<std::string>());
}

void AggregationReplier::FillPortConnectionState(PortConnection& port_connection,
                                                 const DatabaseClient::port_graphml_pair& port_pair,
                                                 PortConnection::ConnectionType type)
{
    port_connection.set_from_port_graphml(port_pair.first);
    port_connection.set_to_port_graphml(port_pair.second);
    port_connection.set_type(type);
}

}
