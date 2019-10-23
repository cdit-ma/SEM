#include "aggregationreplier.h"

#include <google/protobuf/util/time_util.h>

using google::protobuf::util::TimeUtil;

namespace detail {
    std::string StringToPSQLTimestamp(const std::string& str) {
        return DatabaseClient::StringToPSQLTimestamp(str);
    }

    class ConditionPairList {
    private:
        std::vector<std::string> cols_;
        std::vector<std::string> vals_;
    public:
        void addPair(const std::string& col_name, const std::string& val) {
            cols_.emplace_back(col_name);
            vals_.emplace_back(val);
        }
        const std::vector<std::string>& getColumns() const {
            return cols_;
        }
        const std::vector<std::string>& getValues() const {
            return vals_;
        }
    };

    class ConditionPairBuilder {
    private:
        ConditionPairList pair_list_;
        ConditionPairBuilder() = default;
    public:
        static ConditionPairBuilder createConditionPairs() {
            return ConditionPairBuilder();
        }
        ConditionPairBuilder& add(const std::string& col_name, const std::vector<std::string>& values) {
            for (const auto& val : values) {
                pair_list_.addPair(col_name, val);
            }
            return *this;
        }
        ConditionPairBuilder& add(const std::string& col_name, const google::protobuf::RepeatedPtrField<std::string>& values) {
            for (const auto& val : values) {
                pair_list_.addPair(col_name, val);
            }
            return *this;
        }
        ConditionPairList finish() {
            return pair_list_;
        }
    };
}
using namespace detail;

AggServer::AggregationReplier::AggregationReplier(std::shared_ptr<DatabaseClient> database) :
    database_(database)
{
    RegisterCallbacks();
    // const auto&& res = database_->GetPortLifecycleEventInfo(
    //     "1970-01-01T00:00:00.000000Z",
    //     "1970-01-01T9:10:23.924073Z",
    //     {"Port.Path"},
    //     {"ComponentAssembly.0//SubscriberPort"}
    // );
    /*const auto&& res = database_->GetWorkloadEventInfo(
        "1970-01-01T00:00:00.000000Z",
        "2020-01-01T9:10:23.924073Z",
        {"WorkerInstance.Path"},
        {"TestAssembly.1/ReceiverInstance/Utility_Worker"}
    );
    std::cout << "num affected rows" << res.size() << std::endl;
    */

   /*
   AggServer::MarkerRequest request;
   request.set_experiment_run_id(25);
   std::cout << "About to process request..." << std::endl;
   const auto& response = ProcessMarkerRequest(request);
   std::cout << "Response info:" << std::endl;
   std::cout << response->DebugString() << std::endl;
   */
}

void AggServer::AggregationReplier::RegisterCallbacks() {
    RegisterProtoCallback<ExperimentRunRequest, ExperimentRunResponse>(
        "GetExperimentRuns",
        std::bind(&AggregationReplier::ProcessExperimentRunRequest, this, std::placeholders::_1)
    );
    RegisterProtoCallback<ExperimentStateRequest, ExperimentStateResponse>(
        "GetExperimentState",
        std::bind(&AggregationReplier::ProcessExperimentStateRequest, this, std::placeholders::_1)
    );
    RegisterProtoCallback<PortLifecycleRequest, PortLifecycleResponse>(
        "GetPortLifecycle",
        std::bind(&AggregationReplier::ProcessPortLifecycleRequest, this, std::placeholders::_1)
    );
    RegisterProtoCallback<PortEventRequest, PortEventResponse>(
        "GetPortEvent",
        std::bind(&AggregationReplier::ProcessPortEventRequest, this, std::placeholders::_1));
    RegisterProtoCallback<WorkloadRequest, WorkloadResponse>(
        "GetWorkload",
        std::bind(&AggregationReplier::ProcessWorkloadEventRequest, this, std::placeholders::_1)
    );
    RegisterProtoCallback<MarkerRequest, MarkerResponse>(
        "GetMarkers",
        std::bind(&AggregationReplier::ProcessMarkerRequest, this, std::placeholders::_1)
    );
    RegisterProtoCallback<CPUUtilisationRequest, CPUUtilisationResponse>(
        "GetCPUUtilisation",
        std::bind(&AggregationReplier::ProcessCPUUtilisationRequest, this, std::placeholders::_1)
    );
    RegisterProtoCallback<MemoryUtilisationRequest, MemoryUtilisationResponse>(
        "GetMemoryUtilisation",
        std::bind(&AggregationReplier::ProcessMemoryUtilisationRequest, this, std::placeholders::_1)
    );
}

std::unique_ptr<AggServer::ExperimentRunResponse>
AggServer::AggregationReplier::ProcessExperimentRunRequest(const AggServer::ExperimentRunRequest& message) {
    std::unique_ptr<ExperimentRunResponse> response(new ExperimentRunResponse());

    const std::string& exp_name = message.experiment_name();
    std::vector<std::pair<std::string, int> > exp_name_id_pairs;

    if (exp_name == "") {
        // If no name is provided ask the database for the list of all names and ExperimentIDs
        const auto& results = database_->GetValues(
            "Experiment",
            {"Name", "ExperimentID"}
        );
        for (const auto& row : results) {
            const auto& name = row.at("Name").as<std::string>();
            const auto& id = row.at("ExperimentID").as<int>();
            exp_name_id_pairs.emplace_back(std::make_pair(name, id));
        }
    } else {
        /*int id = database_->GetID(
            "Experiment",
            "Name LIKE " + database_->EscapeString("%"+exp_name+"%")
        );
        exp_name_id_pairs.emplace_back(std::make_pair(exp_name, id));*/
        const auto& results = database_->GetValues(
            "Experiment",
            {"Name", "ExperimentID"},
            "Name LIKE " + database_->EscapeString("%"+exp_name+"%")
        );
        for (const auto& row : results) {
            const auto& name = row.at("Name").as<std::string>();
            const auto& id = row.at("ExperimentID").as<int>();
            exp_name_id_pairs.emplace_back(std::make_pair(name, id));
        }
    }

    for (const auto& pair : exp_name_id_pairs) {
        const auto& name = pair.first;
        const auto& id = pair.second;
        auto exp_info = response->add_experiments();
        exp_info->set_name(name);

        const auto& results = database_->GetValues(
            "ExperimentRun",
            {
                "ExperimentRunID",
                "JobNum",
                StringToPSQLTimestamp("StartTime")+" AS StartTime",
                StringToPSQLTimestamp("EndTime")+" AS EndTime"},
            "ExperimentID = " + std::to_string(id)
        );

        for (const auto& row : results) {
            auto run = exp_info->add_runs();
            run->set_experiment_run_id(row.at("ExperimentRunID").as<int>());
            run->set_job_num(row.at("JobNum").as<int>());
            std::string start_time_str = row.at("StartTime").as<std::string>();
            std::cout << start_time_str << std::endl;
            bool success = TimeUtil::FromString(start_time_str, run->mutable_start_time());
            if (!success) {
                throw std::runtime_error("Unable to parse ExperimentRun.StartTime from in-database string representation");
            }
            try {
                std::string end_time_str = row.at("EndTime").as<std::string>();
                TimeUtil::FromString(end_time_str, run->mutable_end_time());
            } catch (const std::exception& e) {
                std::cerr << "An exception occurred while querying experiment run end time, likely due to null: " << e.what() << "\n";
            }
        }
    }

    return response;
}

std::unique_ptr<AggServer::ExperimentStateResponse>
AggServer::AggregationReplier::ProcessExperimentStateRequest(const AggServer::ExperimentStateRequest& message) {
    std::unique_ptr<ExperimentStateResponse> response(new ExperimentStateResponse());

    const int run_id = message.experiment_run_id();

    try {
        const auto& run_results = database_->GetValues(
            "ExperimentRun",
            {
                "JobNum",
                StringToPSQLTimestamp("StartTime")+" AS StartTime",
                StringToPSQLTimestamp("EndTime")+" AS EndTime",
                StringToPSQLTimestamp("LastUpdated")+" AS LastUpdated"
            },
            "ExperimentRunID = " + std::to_string(run_id)
        );

        // 0 results indicates experiment run, more than one means conflicting experiment runs exist
        if (run_results.size() == 1) {
            try {
                const auto& last_updated = run_results.at(0).at("LastUpdated").as<std::string>();
                TimeUtil::FromString(last_updated, response->mutable_last_updated()); 
            } catch (const std::exception& e) {
                std::cerr << "Unable to retrieve the last time experiment run was updated:\n" << 
                    e.what() << std::endl;
            }
            try {
                const auto& last_updated = run_results.at(0).at("EndTime").as<std::string>();
                TimeUtil::FromString(last_updated, response->mutable_end_time()); 
            } catch (const std::exception& e) {
                std::cerr << "Unable to retrieve the shutdown time of the experiment run:\n" << 
                    e.what() << std::endl;
            }
        } else {
            throw std::runtime_error(
                "Expected exactly one result row when querying DB for experiment state, received "+
                std::to_string(run_results.size())
            );
        }

        const auto& node_results = database_->GetValues(
            "Node",
            {"NodeID", "Hostname", "IP"},
            "ExperimentRunID = "+std::to_string(run_id)
        );
        for (const auto& row : node_results) {
            FillNodeState(*response->add_nodes(), row, run_id);
        }

        const auto& component_results = database_->GetValues(
            "Component",
            {"Name"},
            "ExperimentRunID = "+std::to_string(run_id)
        );
        for (const auto& row : component_results) {
            auto new_component = response->add_components();
            new_component->set_name(row.at("Name").as<std::string>());
        }

        const auto& worker_results = database_->GetValues(
            "Worker",
            {"Name"},
            "ExperimentRunID = "+std::to_string(run_id)
        );
        for (const auto& row : worker_results) {
            auto new_component = response->add_workers();
            new_component->set_name(row.at("Name").as<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "An exception occured while populating ExperimentStateResponse with ExperimentRunID=" << run_id << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<AggServer::PortLifecycleResponse>
AggServer::AggregationReplier::ProcessPortLifecycleRequest(const AggServer::PortLifecycleRequest& message) {

    std::unique_ptr<AggServer::PortLifecycleResponse> response = std::unique_ptr<AggServer::PortLifecycleResponse>(
        new AggServer::PortLifecycleResponse()
    );

    std::string start, end;

    // Start time defaults to 0 if not specified
    if (message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {

        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if (message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    const auto conditions = ConditionPairBuilder::createConditionPairs()
        .add("Port.Path", message.port_paths())
        .add("Port.GraphmlID", message.port_ids())
        .add("ComponentInstance.Path", message.component_instance_paths())
        .add("ComponentInstance.GraphmlID", message.component_instance_ids())
        .add("Component.Name", message.component_names())
        .finish();
    /*std::vector<std::string> condition_cols;
    std::vector<std::string> condition_vals;
    for(const auto& port_path : message.port_paths()) {
        condition_cols.emplace_back("Port.path");
        condition_vals.emplace_back(port_path);
    }
    for(const auto& port_id : message.port_ids()) {
        condition_cols.emplace_back("Port.GraphmlID");
        condition_vals.emplace_back(port_id);
    }
    for(const auto& component_inst_path : message.component_instance_paths()) {
        condition_cols.emplace_back("ComponentInstance.Path");
        condition_vals.emplace_back(component_inst_path);
    }
    for(const auto& component_inst_id : message.component_instance_ids()) {
        condition_cols.emplace_back("ComponentInstance.GraphmlID");
        condition_vals.emplace_back(component_inst_id);
    }
    for(const auto& component_name : message.component_names()) {
        condition_cols.emplace_back("Component.Name");
        condition_vals.emplace_back(component_name);
    }*/

    try {
        const pqxx::result res = database_->GetPortLifecycleEventInfo(
            message.experiment_run_id(),
            start,
            end,
            conditions.getColumns(),
            conditions.getValues()
            //condition_cols,
            //condition_vals
        );

        for (const auto& row : res) {
            auto event = response->add_events();

            // Build Event
            auto&& type_int = row["Type"].as<int>();
            event->set_type((AggServer::LifecycleType)type_int);
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if (!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "+timestamp_str);
            }

            // Build Port
            auto port = event->mutable_port();
            port->set_name(row["PortName"].as<std::string>());
            port->set_path(row["PortPath"].as<std::string>());
            Port::Kind kind;
            bool did_parse_lifecycle = AggServer::Port::Kind_Parse(row["PortKind"].as<std::string>(), &kind);
            if (!did_parse_lifecycle) {
                throw std::runtime_error("Failed to parse Lifecycle Kind field from string: "+row["PortKind"].as<std::string>());
            }
            port->set_kind(kind);
            port->set_middleware(row["Middleware"].as<std::string>());
            port->set_graphml_id(row["PortGraphmlID"].as<std::string>());

        }

    } catch (const std::exception& ex) {
        std::cerr << "An exception occurred while querying PortLifecycleEvents:" << ex.what() << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<AggServer::PortEventResponse>
AggServer::AggregationReplier::ProcessPortEventRequest(const AggServer::PortEventRequest& message)
{
    auto response = std::make_unique<AggServer::PortEventResponse>();

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
        const pqxx::result res = database_->GetPortEventInfo(message.experiment_run_id(), start,
                                                             end, conditions.getColumns(),
                                                             conditions.getValues());

        for(const auto& row : res) {
            auto event = response->add_events();

            // Build Event
            auto&& db_type_str = row["Type"].as<std::string>();
            AggServer::PortEvent::PortEventType event_type;
            bool did_parse_type = AggServer::PortEvent_PortEventType_Parse(db_type_str,
                                                                           &event_type);
            if(!did_parse_type) {
                throw std::runtime_error("Failed to parse PortEventType from string: "
                                         + db_type_str);
            }
            event->set_type(event_type);
            event->set_message(row["Message"].as<std::string>());
            event->set_port_event_id(row["SequenceNum"].as<int>());
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
            bool did_parse_lifecycle =
                AggServer::Port::Kind_Parse(row["PortKind"].as<std::string>(), &kind);
            if(!did_parse_lifecycle) {
                throw std::runtime_error("Failed to parse Lifecycle Kind field from string: "
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

std::unique_ptr<AggServer::WorkloadResponse>
AggServer::AggregationReplier::ProcessWorkloadEventRequest(const AggServer::WorkloadRequest& message) {

    std::unique_ptr<AggServer::WorkloadResponse> response = std::unique_ptr<AggServer::WorkloadResponse>(
        new AggServer::WorkloadResponse()
    );

    std::string start, end;

    // Start time defaults to 0 if not specified
    if (message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {

        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if (message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    const auto conditions = ConditionPairBuilder::createConditionPairs()
        .add("WorkerInstance.Path", message.worker_paths())
        .add("Worker.GraphmlID", message.worker_instance_ids())
        .add("ComponentInstance.Path", message.component_instance_paths())
        .add("ComponentInstance.GraphmlID", message.component_instance_ids())
        .add("Component.Name", message.component_names())
        .finish();

    try {
        const pqxx::result res = database_->GetWorkloadEventInfo(
            message.experiment_run_id(),
            start,
            end,
            conditions.getColumns(),
            conditions.getValues()
        );

        for (const auto& row : res) {
            auto event = response->add_events();

            // Build Event
            WorkloadEvent::WorkloadEventType type;
            std::string type_string = row["Type"].as<std::string>();
            bool did_parse_workloadtype = WorkloadEvent::WorkloadEventType_Parse(type_string, &type);
            if (!did_parse_workloadtype) {
                // Workaround for string mismatch due to Windows being unable to handle ERROR as a name (LOG-94)
                if (type_string == "ERROR") {
                    type = WorkloadEvent::ERROR_EVENT;
                } else {
                    throw std::runtime_error("Unable to parse WorkloadEventType from string: "+type_string);
                }
            }
            event->set_type(type);
            //event->set_type((AggServer::WorkloadEvent::WorkloadEventType)type_int);
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if (!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "+timestamp_str);
            }
            event->set_function_name(row["FunctionName"].as<std::string>());
            event->set_args(row["Arguments"].as<std::string>());

            // Build Port
            auto worker_inst = event->mutable_worker_inst();
            worker_inst->set_name(row["WorkerInstanceName"].as<std::string>());
            worker_inst->set_path(row["WorkerInstancePath"].as<std::string>());
            worker_inst->set_graphml_id(row["WorkerInstanceGraphmlID"].as<std::string>());

        }

    } catch (const std::exception& ex) {
        std::cerr << "An exception occurred while querying WorkloadEvents:" << ex.what() << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<AggServer::MarkerResponse>
AggServer::AggregationReplier::ProcessMarkerRequest(const AggServer::MarkerRequest& message) {
    std::unique_ptr<AggServer::MarkerResponse> response = std::unique_ptr<AggServer::MarkerResponse>(
        new AggServer::MarkerResponse()
    );

    std::string start, end;

    // Start time defaults to 0 if not specified
    if (message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {
        start = "";
        //start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if (message.time_interval_size() >= 2) {
        end = TimeUtil::ToString(message.time_interval()[1]);
    } else {
        end = "";
        //end = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // Get filter conditions
    const auto conditions = ConditionPairBuilder::createConditionPairs()
        .add("WorkerInstance.Path", message.worker_instance_paths())
        .add("Worker.GraphmlID", message.worker_instance_ids())
        .add("ComponentInstance.Path", message.component_instance_paths())
        .add("ComponentInstance.GraphmlID", message.component_instance_ids())
        .add("Component.Name", message.component_names())
        .finish();

    try {
        const pqxx::result res = database_->GetMarkerInfo(
            message.experiment_run_id(),
            start,
            end,
            conditions.getColumns(),
            conditions.getValues()
        );

        std::unordered_map<std::string, AggServer::MarkerNameSet*> name_set_map;
        std::unordered_map<std::string, AggServer::MarkerIDSet*> id_set_map;

        for (const auto& row : res) {
            // Get label and create new name_set if one doesn't already exist
            const std::string& label = row["Label"].as<std::string>();
            AggServer::MarkerNameSet* name_set = nullptr;
            try {
                name_set = name_set_map.at(label);
            } catch (const std::out_of_range& oor_exception) {
                name_set = response->add_marker_name_sets();
                name_set->set_name(label);
                name_set_map.emplace(label, name_set);
            }

            // Get id and create new id_set if one doesn't already exist for the given name+id combo
            int id = row["WorkloadID"].as<int>();
            std::string id_str = label + '_' + std::to_string(id);
            AggServer::MarkerIDSet* id_set = nullptr;
            try {
                id_set = id_set_map.at(id_str);
            } catch (const std::out_of_range& oor_exception) {
                id_set = name_set->add_marker_id_set();
                id_set->set_id(id);
                id_set_map.emplace(id_str, id_set);
            }

            // Build MarkerEvent
            AggServer::MarkerEvent* event = id_set->add_events();
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_timestamp());
            if (!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "+timestamp_str);
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

    } catch (const std::exception& ex) {
        std::cerr << "An exception occurred while querying MarkerEvents:" << ex.what() << std::endl;
        throw;
    }

    return response;
}

std::unique_ptr<AggServer::CPUUtilisationResponse>
AggServer::AggregationReplier::ProcessCPUUtilisationRequest(const AggServer::CPUUtilisationRequest& message) {
    std::unique_ptr<CPUUtilisationResponse> response(new CPUUtilisationResponse());

    std::string start, end;

    // Start time defaults to 0 if not specified
    if (message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {

        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if (message.time_interval_size() >= 2) {
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
        AggServer::CPUUtilisationNode* current_node;
        const pqxx::result res = database_->GetCPUUtilInfo(
            message.experiment_run_id(),
            start,
            end,
            condition_cols,
            condition_vals
        );

        for (const auto& row : res) {
            // Check if we need to create a new Node due to encoutnering a new hostname
            std::string hostname = row["NodeHostname"].as<std::string>();
            if (current_hostname != hostname) {
                current_hostname = hostname;
                current_node = response->add_nodes();
                current_node->mutable_node_info()->set_hostname(hostname);
                current_node->mutable_node_info()->set_ip(row["NodeIP"].as<std::string>());
            }
            auto event = current_node->add_events();

            // Build Event
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if (!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "+timestamp_str);
            }
            event->set_cpu_utilisation(row["CPUUtilisation"].as<double>());

        }
    } catch (const std::exception& ex) {
        std::cerr << "An exception occurred while querying CPUUtilisationEvents:" << ex.what() << std::endl;
        throw;
    }


    return response;
}

std::unique_ptr<AggServer::MemoryUtilisationResponse>
AggServer::AggregationReplier::ProcessMemoryUtilisationRequest(const AggServer::MemoryUtilisationRequest& message) {
    std::unique_ptr<MemoryUtilisationResponse> response(new MemoryUtilisationResponse());

    std::string start, end;

    // Start time defaults to 0 if not specified
    if (message.time_interval_size() >= 1) {
        start = TimeUtil::ToString(message.time_interval()[0]);
    } else {

        start = TimeUtil::ToString(TimeUtil::SecondsToTimestamp(0));
    }

    // End time defaults to 0 if not specified
    if (message.time_interval_size() >= 2) {
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
        AggServer::MemoryUtilisationNode* current_node;
        const pqxx::result res = database_->GetMemUtilInfo(
            message.experiment_run_id(),
            start,
            end,
            condition_cols,
            condition_vals
        );

        for (const auto& row : res) {
            // Check if we need to create a new Node due to encoutnering a new hostname
            std::string hostname = row["NodeHostname"].as<std::string>();
            if (current_hostname != hostname) {
                current_hostname = hostname;
                current_node = response->add_nodes();
                current_node->mutable_node_info()->set_hostname(hostname);
                current_node->mutable_node_info()->set_ip(row["NodeIP"].as<std::string>());
            }
            auto event = current_node->add_events();

            // Build Event
            auto&& timestamp_str = row["SampleTime"].as<std::string>();
            bool did_parse = TimeUtil::FromString(timestamp_str, event->mutable_time());
            if (!did_parse) {
                throw std::runtime_error("Failed to parse SampleTime field from string: "+timestamp_str);
            }
            event->set_memory_utilisation(row["PhysMemUtilisation"].as<double>());

        }
    } catch (const std::exception& ex) {
        std::cerr << "An exception occurred while querying CPUUtilisationEvents:" << ex.what() << std::endl;
        throw;
    }


    return response;
}


void AggServer::AggregationReplier::FillNodeState(
    AggServer::Node& node,
    const pqxx::row& node_values,
    int experiment_run_id
) {

    auto node_id = node_values.at("NodeID").as<std::string>();
    node.set_hostname(node_values.at("Hostname").as<std::string>());
    node.set_ip(node_values.at("IP").as<std::string>());

    try {
        const auto& results = database_->GetValues(
            "Container",
            {"ContainerID", "Name", "Type"},
            "NodeID = " + node_id
        );
        for (const auto& row : results) {
            FillContainerState(*node.add_containers(), row, experiment_run_id);
        }
    } catch (const std::exception& e) {
        std::cerr << "An exception occured while populating ExperimentStateResponse Node with NodeID=" << node_id << std::endl;
        throw;
    }
}

void AggServer::AggregationReplier::FillContainerState(
    AggServer::Container& container,
    const pqxx::row& container_values,
    int experiment_run_id
) {
    auto container_id = container_values.at("ContainerID").as<std::string>();
    container.set_name(container_values.at("Name").as<std::string>());
    Container::ContainerType type;
    Container::ContainerType_Parse(container_values.at("Type").as<std::string>(), &type);
    container.set_type(type);

    try {
        const auto& results = database_->GetValues(
            "ComponentInstance",
            {"ComponentInstanceID", "Name", "ComponentID", "Path", "GraphmlID"},
            "ComponentID = " + container_id
        );
        for (const auto& row : results) {
            FillComponentInstanceState(*container.add_component_instances(), row, experiment_run_id);
        }
    } catch (const std::exception& e) {
        std::cerr << "An exception occured while populating ExperimentStateResponse Container with ContainerID=" << container_id << std::endl;
        throw;
    }
}

void AggServer::AggregationReplier::FillComponentInstanceState(
    AggServer::ComponentInstance& component_instance,
    const pqxx::row& component_instance_values,
    int experiment_run_id
) {

    auto component_instance_id = component_instance_values.at("ComponentInstanceID").as<std::string>();
    component_instance.set_name(component_instance_values.at("Name").as<std::string>());
    component_instance.set_path(component_instance_values.at("Path").as<std::string>());
    component_instance.set_graphml_id(component_instance_values.at("GraphmlID").as<std::string>());

    const std::string& component_id = component_instance_values.at("ComponentID").as<std::string>();

    try {
        const auto& component_results = database_->GetValues(
            "Component",
            {"Name"},
            "ComponentID = " + component_id + " AND ExperimentRunID = " + std::to_string(experiment_run_id)
        );
        for (const auto& row : component_results) {
            component_instance.set_type(row.at("Name").as<std::string>());
        }

        const auto& port_results = database_->GetValues(
            "Port",
            {"Name", "Kind", "Path", "GraphmlID", "Middleware"},
            "ComponentInstanceID = " + component_instance_id
        );
        for (const auto& row : port_results) {
            FillPortState(*component_instance.add_ports(), row);
        }

        const auto& worker_instance_results = database_->GetValues(
            "WorkerInstance",
            {"Name", "WorkerID", "Path", "GraphmlID"},
            "ComponentInstanceID = " + component_instance_id
        );
        for (const auto& row : worker_instance_results) {
            FillWorkerInstanceState(*component_instance.add_worker_instances(), row, experiment_run_id);
        }
    } catch (const std::exception& e) {
        std::cerr << "An exception occured while populating ExperimentStateResponse ComponentInstance with ID=" << component_instance_id << std::endl;
        throw;
    }
}

void AggServer::AggregationReplier::FillWorkerInstanceState(
    AggServer::WorkerInstance& worker_instance,
    const pqxx::row& worker_instance_values,
    int experiment_run_id
) {
    worker_instance.set_name(worker_instance_values.at("Name").as<std::string>());
    worker_instance.set_path(worker_instance_values.at("Path").as<std::string>());
    worker_instance.set_graphml_id(worker_instance_values.at("GraphmlID").as<std::string>());
    const std::string& worker_id = worker_instance_values.at("WorkerID").as<std::string>();

    const auto& worker_results = database_->GetValues(
        "Worker",
        {"Name"},
        "WorkerID = " + worker_id + " AND ExperimentRunID = " + std::to_string(experiment_run_id)
    );
    for (const auto& row : worker_results) {
        worker_instance.set_type(row.at("Name").as<std::string>());
    }
}

void AggServer::AggregationReplier::FillPortState(AggServer::Port& port, const pqxx::row& port_values) {
    port.set_name(port_values.at("Name").as<std::string>());
    Port::Kind kind;
    Port::Kind_Parse(port_values.at("Kind").as<std::string>(), &kind);
    port.set_kind(kind);
    port.set_path(port_values.at("Path").as<std::string>());
    port.set_middleware(port_values.at("Middleware").as<std::string>());
    port.set_graphml_id(port_values.at("GraphmlID").as<std::string>());
}