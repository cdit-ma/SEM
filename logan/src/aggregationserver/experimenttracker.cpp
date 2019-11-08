#include "experimenttracker.h"

#include <iostream>
#include <sstream>

#include <proto/controlmessage/controlmessage.pb.h>

#include "modeleventprotohandler.h"
#include "systemeventprotohandler.h"

#include "databaseclient.h"

#include "utils.h"
#include <google/protobuf/util/time_util.h>

#include <zmq/protoreceiver/protoreceiver.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::time_point;

ExperimentTracker::ExperimentTracker(std::shared_ptr<DatabaseClient> db_client) :
    database_(db_client)
{
}

int ExperimentTracker::RegisterExperimentRun(const std::string& experiment_name,
                                             const google::protobuf::Timestamp& timestamp)
{
    // TODO: quick fix for demo, should drive time value off of message once it is being populated
    using google::protobuf::util::TimeUtil;
    // std::string start_time = TimeUtil::ToString(TimeUtil::GetCurrentTime());
    std::string start_time = database_->quote(TimeUtil::ToString(timestamp));

    std::cout << "Registering experiment with name " << experiment_name << " @ time " << timestamp
              << std::endl;

    std::lock_guard<std::mutex> access_lock(access_mutex_);

    int experiment_id;
    try {
        experiment_id = experiment_name_cache_.at(experiment_name);
    } catch(const std::exception& oor_exception) {
        const std::vector<std::string> columns = {"Name", "ModelName"};
        const std::vector<std::string> values = {experiment_name, experiment_name};
        experiment_id = database_->InsertValuesUnique("Experiment", columns, values, {"Name"});
        // ID of -1 indicates an invalid experiment
        if(experiment_id == -1) {
            throw std::runtime_error(std::string("Unable to register experiment with name ")
                                     + experiment_name);
        }
        experiment_name_cache_.emplace(std::make_pair(experiment_name, experiment_id));
    }

    if(active_experiment_ids_.count(experiment_id)) {
        return active_experiment_ids_.at(experiment_id);
    } else {
        const auto& max_val = database_->GetMaxValue(
            "ExperimentRun", "JobNum", "ExperimentID=" + database_->quote(experiment_id));
        ExperimentRunInfo new_run;
        new_run.name = experiment_name;
        if(max_val.has_value()) {
            new_run.job_num = *max_val + 1;
        } else {
            new_run.job_num = 0;
        }
        new_run.running = true;

        new_run.experiment_run_id = database_->InsertValues(
            "ExperimentRun", {"ExperimentID", "JobNum", "StartTime"},
            {database_->quote(experiment_id), database_->quote(new_run.job_num), start_time});

        new_run.receiver = std::unique_ptr<zmq::ProtoReceiver>(new zmq::ProtoReceiver());
        new_run.system_handler = std::unique_ptr<SystemEventProtoHandler>(
            new SystemEventProtoHandler(database_, *this, new_run.experiment_run_id));
        new_run.model_handler = std::unique_ptr<ModelEventProtoHandler>(
            new ModelEventProtoHandler(database_, *this, new_run.experiment_run_id));
        new_run.system_handler->BindCallbacks(*new_run.receiver);
        new_run.model_handler->BindCallbacks(*new_run.receiver);
        new_run.receiver->Filter("");

        experiment_run_map_.emplace(new_run.experiment_run_id, std::move(new_run));
        active_experiment_ids_.emplace(std::make_pair(experiment_id, new_run.experiment_run_id));

        return new_run.experiment_run_id;
    }
}

void ExperimentTracker::ShutdownExperimentRun(const std::string& experiment_name,
                                              const google::protobuf::Timestamp& timestamp)
{
    std::cout << "Shuting down experiment with name " << experiment_name << std::endl;
    int experiment_id = GetExperimentID(experiment_name);
    int experiment_run_id = GetCurrentRunID(experiment_id);

    using google::protobuf::util::TimeUtil;
    std::string end_time = TimeUtil::ToString(timestamp);
    database_->UpdateShutdownTime(experiment_run_id, end_time);

    active_experiment_ids_.erase(experiment_id);
    auto& run = GetExperimentRunInfo(experiment_run_id);
    run.running = false;
}

void ExperimentTracker::RegisterSystemEventProducer(int experiment_run_id,
                                                    const std::string& endpoint)
{
    std::cerr << "Connecting: " << experiment_run_id
              << " TO RegisterSystemEventProducer EVENT: " << endpoint << std::endl;
    GetExperimentRunInfo(experiment_run_id).receiver->Connect(endpoint);
}
void ExperimentTracker::RegisterModelEventProducer(int experiment_run_id,
                                                   const std::string& endpoint)
{
    std::cerr << "Connecting: " << experiment_run_id
              << " TO RegisterModelEventProducer EVENT: " << endpoint << std::endl;
    GetExperimentRunInfo(experiment_run_id).receiver->Connect(endpoint);
}

int ExperimentTracker::GetExperimentID(const std::string& experiment_name)
{
    return experiment_name_cache_.at(experiment_name);
}

int ExperimentTracker::GetCurrentRunID(int experiment_id)
{
    return active_experiment_ids_.at(experiment_id);
}

void ExperimentTracker::AddNodeIDWithHostname(int experiment_run_id,
                                              const std::string& hostname,
                                              int node_id)
{
    GetExperimentRunInfo(experiment_run_id)
        .hostname_node_id_cache.emplace(std::make_pair(hostname, node_id));
}

int ExperimentTracker::GetNodeIDFromHostname(int experiment_run_id, const std::string& hostname)
{
    auto& exp_info = GetExperimentRunInfo(experiment_run_id);
    try {
        return exp_info.hostname_node_id_cache.at(hostname);
    } catch(const std::out_of_range& oor_exception) {
        // If we can't find anything then go back to the database
        std::stringstream condition_stream;
        condition_stream << "Hostname = " << database_->quote(hostname)
                         << " AND ExperimentRunID = " << experiment_run_id;
        const auto& node_id = database_->GetID("Node", condition_stream.str());
        AddNodeIDWithHostname(experiment_run_id, hostname, node_id);
        return node_id;
    }
}

ExperimentRunInfo& ExperimentTracker::GetExperimentRunInfo(int experiment_run_id)
{
    try {
        return experiment_run_map_.at(experiment_run_id);
    } catch(const std::exception&) {
        throw std::invalid_argument("No registered experiment run with ID: '"
                                    + std::to_string(experiment_run_id) + "'");
    }
}
