#ifndef MODELEVENTPROTOHANDLER_H
#define MODELEVENTPROTOHANDLER_H

#include "aggregationprotohandler.h"

#include <proto/modelevent/modelevent.pb.h>

#include <map>

class ModelEventProtoHandler : public AggregationProtoHandler {
public:
    ModelEventProtoHandler(std::shared_ptr<DatabaseClient> db_client, ExperimentTracker& exp_tracker, int experiment_run_id)
        : AggregationProtoHandler(db_client, exp_tracker), experiment_run_id_(experiment_run_id) {};

    void BindCallbacks(zmq::ProtoReceiver& ProtoReceiver);

private:
    // Model callbacks
    //void ProcessUserEvent(const ModelEvent::UserEvent& message);
    void ProcessLifecycleEvent(const ModelEvent::LifecycleEvent& message);
    void ProcessWorkloadEvent(const ModelEvent::WorkloadEvent& message);
    void ProcessUtilizationEvent(const ModelEvent::UtilizationEvent& message);

    // Insertion helpers
    void InsertComponentLifecycleEvent(const ModelEvent::Info& info,
            const ModelEvent::LifecycleEvent_Type& type,
            const ModelEvent::Component& component);

    void InsertPortLifecycleEvent(const ModelEvent::Info& info,
            const ModelEvent::LifecycleEvent_Type& type,
            const ModelEvent::Component& component,
            const ModelEvent::Port& port);

    // ID retrieval helpers
    int GetComponentID(const std::string& name);
    int GetComponentInstanceID(const ModelEvent::Component& component_instance);
    int GetPortID(const ModelEvent::Port& port, const ModelEvent::Component& component);
    int GetWorkerInstanceID(const ModelEvent::Component& component_instance, const ModelEvent::Worker& worker_instance);


    int experiment_run_id_;

    std::map<std::string, int> component_id_cache_;
    std::map<std::string, int> component_inst_id_cache_;
    std::map<std::string, int> port_id_cache_;
    std::map<std::string, int> worker_inst_id_cache_;
};

#endif