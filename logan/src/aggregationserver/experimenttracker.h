#ifndef EXPERIMENTTRACKER_H
#define EXPERIMENTTRACKER_H

#include <string>
#include <exception>
#include <memory>
#include <mutex>

#include <map>
#include <set>

#include <google/protobuf/timestamp.pb.h>

#include <zmq/protoreceiver/protoreceiver.h>

#include "systemeventprotohandler.h"
#include "modeleventprotohandler.h"

class DatabaseClient;

namespace NodeManager {
    class ControlMessage;
}

struct ExperimentRunInfo {
    std::string name;
    int job_num;
    bool running;
    int experiment_run_id;
    std::unique_ptr<zmq::ProtoReceiver> receiver;
    std::unique_ptr<SystemEventProtoHandler> system_handler;
    std::unique_ptr<ModelEventProtoHandler> model_handler;

    std::map<std::string, int> hostname_node_id_cache;
};

class ExperimentTracker {
public:
    ExperimentTracker(std::shared_ptr<DatabaseClient> db_client);
    int RegisterExperimentRun(
        const std::string& experiment_name,
        const google::protobuf::Timestamp& timestamp
    );
    void ShutdownExperimentRun(
        const std::string& experiment_name,
        const google::protobuf::Timestamp& timestamp
    );

    void RegisterSystemEventProducer(int experiment_run_id, const std::string& endpoint);
    void RegisterModelEventProducer(int experiment_run_id, const std::string& endpoint);

    int GetExperimentID(const std::string& experiment_name);
    int GetCurrentRunID(int experiment_id);

    void AddNodeIDWithHostname(int experiment_run_id, const std::string& hostname, int node_id);
    int GetNodeIDFromHostname(int experiment_run_id, const std::string& hostname);

private:
    ExperimentRunInfo& GetExperimentRunInfo(int experiment_run_id);
    

    std::shared_ptr<DatabaseClient> database_;
    std::map<int, ExperimentRunInfo> experiment_run_map_;
    std::map<int, int> active_experiment_ids_;  // Maps ExperimentID -> current ExperimentRunID

    std::map<std::string, int> experiment_name_cache_; // Maps Experiment.Name to ExperimentID

    std::mutex access_mutex_;
};

#endif