#ifndef SYSTEMEVENTPROTOHANDLER_H
#define SYSTEMEVENTPROTOHANDLER_H

#include "aggregationprotohandler.h"

#include <proto/systemevent/systemevent.pb.h>

#include <map>
#include <utility>

class SystemEventProtoHandler : public AggregationProtoHandler {
public:
    SystemEventProtoHandler(std::shared_ptr<DatabaseClient> db_client,
                            ExperimentTracker& exp_tracker,
                            int experiment_run_id) :
        AggregationProtoHandler(std::move(db_client), exp_tracker),
        experiment_run_id_(experiment_run_id){};

    void BindCallbacks(zmq::ProtoReceiver& ProtoReceiver) final;

private:
    // Hardware callbacks
    void ProcessStatusEvent(const SystemEvent::StatusEvent& status);

    void ProcessInterfaceStatus(const SystemEvent::InterfaceStatus& if_status,
                                const std::string& hostname,
                                const std::string& timestamp);

    void ProcessFileSystemStatus(const SystemEvent::FileSystemStatus& fs_status,
                                 const std::string& hostname,
                                 const std::string& timestamp);

    void ProcessProcessStatus(const SystemEvent::ProcessStatus& p_status,
                              const std::string& hostname,
                              const std::string& timestamp);

    void ProcessDeviceMetricSamples(const SystemEvent::DeviceMetricSamples& dev_samples);

    void ProcessGPUMetricSample(const SystemEvent::GPUMetricSample& gpu_sample,
                                const std::string& hostname);

    void ProcessInfoEvent(const SystemEvent::InfoEvent& info);

    void ProcessFileSystemInfo(const SystemEvent::FileSystemInfo& fs_info,
                               const std::string& hostname,
                               int node_id);

    void ProcessInterfaceInfo(const SystemEvent::InterfaceInfo& i_info,
                              const std::string& hostname,
                              int node_id);

    void ProcessProcessInfo(const SystemEvent::ProcessInfo& p_info, const std::string& hostname);

    void ProcessDeviceInfo(const SystemEvent::DeviceInfo& d_info, int node_id);

    void ProcessGPUDescriptor(const SystemEvent::GPUDescriptor& gpu_info,
                              const std::string& hostname,
                              int node_id);

    // ID lookup key generator helper functions
    [[nodiscard]] std::string
    GetInterfaceKey(const std::string& hostname, const std::string& if_name) const;

    [[nodiscard]] std::string
    GetFileSystemKey(const std::string& hostname, const std::string& fs_name) const;

    [[nodiscard]] std::string
    GetProcessKey(const std::string& hostname, int pid, const std::string& starttime) const;

    [[nodiscard]] std::string
    GetGPUKey(const std::string& hostname, const std::string& gpu_name) const;

    // Experiment info
    int experiment_run_id_;

    // Cache maps
    std::map<std::string, int> system_id_cache_;     // hostname -> SystemID
    std::map<std::string, int> filesystem_id_cache_; // hostname/fs_name -> FileSystemID
    std::map<std::string, int> interface_id_cache_;  // hostname/if_name -> InterfaceID
    std::map<std::string, int> process_id_cache_;    // hostname/pID/starttime -> InterfaceID
    std::map<std::string, int> gpu_id_cache_;        // hostname/gpu_name -> InterfaceID
};

#endif // SYSTEMEVENTPROTOHANDLER_H