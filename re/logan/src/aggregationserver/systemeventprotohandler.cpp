#include "systemeventprotohandler.h"

#include "utils.h"

#include "databaseclient.h"
#include "experimenttracker.h"

#include <zmq/protoreceiver/protoreceiver.h>

#include <google/protobuf/util/time_util.h>

#include <functional>

using google::protobuf::util::TimeUtil;

void SystemEventProtoHandler::BindCallbacks(zmq::ProtoReceiver& receiver)
{
    receiver.RegisterProtoCallback<SystemEvent::StatusEvent>(
        std::bind(&SystemEventProtoHandler::ProcessStatusEvent, this, std::placeholders::_1));
    receiver.RegisterProtoCallback<SystemEvent::InfoEvent>(
        std::bind(&SystemEventProtoHandler::ProcessInfoEvent, this, std::placeholders::_1));
}

void SystemEventProtoHandler::ProcessStatusEvent(const SystemEvent::StatusEvent& event)
{
    const std::string& hostname = event.hostname();
    const std::string& time_str = database_->quote(TimeUtil::ToString(event.timestamp()));
    const auto& cpu_util = database_->quote(event.cpu_utilization());
    const auto& phys_mem = database_->quote(event.phys_mem_utilization());

    std::string system_id = "-1";
    try {
        system_id = database_->quote(system_id_cache_.at(hostname));
    } catch(const std::out_of_range& oor_ex) {
        std::cerr << "Failed to insert StatusEvent for hostname '" << hostname << "', "
                  << oor_ex.what() << "\n";
        throw;
    }

    database_->InsertValues("Hardware.SystemStatus",
                            {"SystemID", "SampleTime", "CPUUtilisation", "PhysMemUtilisation"},
                            {system_id, time_str, cpu_util, phys_mem});

    for(const auto& iface : event.interfaces()) {
        ProcessInterfaceStatus(iface, hostname, time_str);
    }

    for(const auto& fs : event.file_systems()) {
        ProcessFileSystemStatus(fs, hostname, time_str);
    }

    for(const auto& p_info : event.process_info()) {
        ProcessProcessInfo(p_info, hostname);
    }

    for(const auto& p : event.processes()) {
        ProcessProcessStatus(p, hostname, time_str);
    }

    for (const auto& d : event.device_samples()) {
        ProcessDeviceMetricSamples(d);
    }
}

void SystemEventProtoHandler::ProcessInterfaceStatus(const SystemEvent::InterfaceStatus& if_status,
                                                     const std::string& hostname,
                                                     const std::string& timestamp)
{
    const auto& rec_packets = database_->quote(if_status.rx_packets());
    const auto& rec_bytes = database_->quote(if_status.rx_bytes());
    const auto& sent_packets = database_->quote(if_status.tx_packets());
    const auto& sent_bytes = database_->quote(if_status.tx_bytes());

    std::string interface_id = "-1";
    try {
        interface_id = database_->quote(
            interface_id_cache_.at(GetInterfaceKey(hostname, if_status.name())));
    } catch(const std::out_of_range& oor_ex) {
        std::cerr << "Failed to insert InterfaceStatus for hostname '" << hostname << "', "
                  << oor_ex.what() << "\n";
        throw;
    }

    database_->InsertValues("Hardware.InterfaceStatus",
                            {"InterfaceID", "PacketsReceived", "BytesReceived",
                             "PacketsTransmitted", "BytesTransmitted", "SampleTime"},
                            {interface_id, rec_packets, rec_bytes, sent_packets, sent_bytes,
                             timestamp});
    database_->UpdateLastSampleTime(experiment_run_id_, timestamp);
}

void SystemEventProtoHandler::ProcessFileSystemStatus(const SystemEvent::FileSystemStatus& fs_status,
                                                      const std::string& hostname,
                                                      const std::string& timestamp)
{
    const auto& util = database_->quote(fs_status.utilization());

    std::string filesystem_id = "-1";
    try {
        filesystem_id = database_->quote(
            filesystem_id_cache_.at(GetFileSystemKey(hostname, fs_status.name())));
    } catch(const std::out_of_range& oor_ex) {
        std::cerr << "Failed to insert FileSystemStatus for hostname '" << hostname << "', "
                  << oor_ex.what() << "\n";
        throw;
    }

    database_->InsertValues("Hardware.FilesystemStatus",
                            {"FilesystemID", "Utilisation", "SampleTime"},
                            {filesystem_id, util, timestamp});
    database_->UpdateLastSampleTime(experiment_run_id_, timestamp);
}

void SystemEventProtoHandler::ProcessProcessStatus(const SystemEvent::ProcessStatus& p_status,
                                                   const std::string& hostname,
                                                   const std::string& timestamp)
{
    const auto& pid = database_->quote(p_status.pid());
    const auto& name = p_status.name();

    const auto& core_id = database_->quote(p_status.cpu_core_id());
    const auto& cpu_util = database_->quote(p_status.cpu_utilization());
    const auto& phys_mem_util = database_->quote(p_status.phys_mem_utilization());
    const auto& phys_mem_used_kb = database_->quote(p_status.phys_mem_used_kb());

    const auto& threads = database_->quote(p_status.thread_count());
    const auto& disk_read_kb = database_->quote(p_status.disk_read_kilobytes());
    const auto& disk_write_kb = database_->quote(p_status.disk_written_kilobytes());
    const auto& disk_total_kb = database_->quote(p_status.disk_total_kilobytes());

    const auto& cpu_time = database_->quote(TimeUtil::ToString(p_status.cpu_time()));
    const auto& state = database_->quote(SystemEvent::ProcessStatus::State_Name(p_status.state()));
    const auto& start_time =TimeUtil::ToString(p_status.start_time());

    std::string process_id = "-1";
    auto&& process_key = GetProcessKey(hostname, p_status.pid(), start_time);
    try {
        process_id = database_->quote(process_id_cache_.at(process_key));
    } catch(const std::out_of_range& oor_ex) {
        std::cerr << "Failed to insert ProcessStatus with key '" << process_key
                  << "' for hostname '" << hostname << "', "
                  << oor_ex.what() << "\n";
        throw;
    }

    database_->InsertValues(
        "Hardware.ProcessStatus",
        {"ProcessID", "CoreID", "CPUUtilisation", "PhysMemUtilisation", "PhysMemUsedKB",
         "ThreadCount", "DiskRead", "DiskWritten", "DiskTotal", "CPUTime", "State", "SampleTime"},
        {process_id, core_id, cpu_util, phys_mem_util, phys_mem_used_kb, threads, disk_read_kb,
         disk_write_kb, disk_total_kb, cpu_time, state, timestamp});
    database_->UpdateLastSampleTime(experiment_run_id_, timestamp);
}


void SystemEventProtoHandler::ProcessDeviceMetricSamples(
    const SystemEvent::DeviceMetricSamples& dev_samples)
{
    for (const auto& gpu_sample : dev_samples.gpu_samples()) {
        ProcessGPUMetricSample(gpu_sample, dev_samples.hostname());
    }
}



void SystemEventProtoHandler::ProcessGPUMetricSample(const SystemEvent::GPUMetricSample& gpu_sample,
                            const std::string& hostname) {
    const auto& gpu_util = database_->quote(gpu_sample.processor_utilisation_perc());
    const auto& mem_util = database_->quote(gpu_sample.memory_utilisation_mib());
    const auto& temp = database_->quote(gpu_sample.temperature_cel());

    const std::string& timestamp = database_->quote(TimeUtil::ToString(gpu_sample.timestamp()));
    std::string gpu_id = "-1";
    try {
        gpu_id = database_->quote(
            gpu_id_cache_.at(GetGPUKey(hostname, gpu_sample.device_name())));
    } catch(const std::out_of_range& oor_ex) {
        std::cerr << "Failed to insert GPUStatus for hostname '" << hostname << "', "
                  << oor_ex.what() << "\n";
        throw;
    }

    database_->InsertValues("Hardware.GPUStatus",
                            {"FilesystemID", "GPUUtilisation", "MemoryUtilisation", "Temperature", "SampleTime"},
                            {gpu_id, gpu_util, mem_util, temp, timestamp});
    database_->UpdateLastSampleTime(experiment_run_id_, timestamp);
}

void SystemEventProtoHandler::ProcessInfoEvent(const SystemEvent::InfoEvent& info)
{
    // const std::string& timestamp = TimeUtil::ToString(info.timestamp());
    const std::string& hostname = info.hostname();
    const int node_id = experiment_tracker_.GetNodeIDFromHostname(experiment_run_id_, hostname);
    const std::string& node_id_str = std::to_string(node_id);

    const std::string& os_name = info.os_name();
    const std::string& os_arch = info.os_arch();
    const std::string& os_desc = info.os_description();
    const std::string& os_version = info.os_version();
    const std::string& os_vendor = info.os_vendor();
    const std::string& os_vendor_name = info.os_vendor_name();

    const std::string& cpu_model = info.cpu_model();
    const std::string& cpu_vendor = info.cpu_vendor();
    const std::string& cpu_frequency = std::to_string(info.cpu_frequency_hz());

    const std::string& physical_memory = std::to_string(info.physical_memory_kilobytes());

    int system_id = database_->InsertValuesUnique(
        "Hardware.System",
        {"NodeID", "OSName", "OSArch", "OSDescription", "OSVersion", "OSVendor", "OSVendorName",
         "CPUModel", "CPUVendor", "CPUFrequencyHZ", "PhysicalMemoryKB"},
        {node_id_str, os_name, os_arch, os_desc, os_version, os_vendor, os_vendor_name, cpu_model,
         cpu_vendor, cpu_frequency, physical_memory},
        {"NodeID"});

    system_id_cache_.emplace(std::make_pair(hostname, system_id));
    std::cout << "Adding system info for hostname: " << hostname << " with id " << system_id
              << std::endl;

    for(const auto& fs_info : info.file_system_info()) {
        ProcessFileSystemInfo(fs_info, hostname, node_id);
    }

    for(const auto& i_info : info.interface_info()) {
        ProcessInterfaceInfo(i_info, hostname, node_id);
    }

    for(const auto& d_info : info.device_info()) {
        ProcessDeviceInfo(d_info, node_id);
    }
}

void SystemEventProtoHandler::ProcessFileSystemInfo(const SystemEvent::FileSystemInfo& fs_info,
                                                    const std::string& hostname,
                                                    int node_id)
{
    const std::string& name = fs_info.name();
    const std::string& type = SystemEvent::FileSystemInfo_Type_Name(fs_info.type());

    const std::string& size = std::to_string(fs_info.size_kilobytes());

    int filesystem_id = database_->InsertValuesUnique("Hardware.Filesystem",
                                                      {"Name", "Type", "Size", "NodeID"},
                                                      {name, type, size, std::to_string(node_id)},
                                                      {"NodeID", "Name"});

    filesystem_id_cache_.emplace(std::make_pair(GetFileSystemKey(hostname, name), filesystem_id));
}

void SystemEventProtoHandler::ProcessInterfaceInfo(const SystemEvent::InterfaceInfo& i_info,
                                                   const std::string& hostname,
                                                   int node_id)
{
    const std::string& node_id_str = std::to_string(node_id);
    const std::string& name = i_info.name();
    const std::string& type = i_info.type();
    const std::string& desc = i_info.description();
    const std::string& ipv4 = i_info.ipv4_addr();
    const std::string& ipv6 = i_info.ipv6_addr();
    const std::string& mac = i_info.mac_addr();
    const std::string& speed = std::to_string((int64_t)i_info.speed());

    int interface_id = database_->InsertValuesUnique(
        "Hardware.Interface",
        {"NodeID", "Name", "Type", "Description", "IPv4", "IPv6", "MAC", "Speed"},
        {node_id_str, name, type, desc, ipv4, ipv6, mac, speed},
        {"NodeID", "Name"});

    interface_id_cache_.emplace(std::make_pair(GetInterfaceKey(hostname, name), interface_id));
}

void SystemEventProtoHandler::ProcessProcessInfo(const SystemEvent::ProcessInfo& p_info,
                                                 const std::string& hostname)
{
    const auto& pid = std::to_string(p_info.pid());
    const auto& cwd = p_info.cwd();
    const auto& name = p_info.name();
    const auto& args = p_info.args();
    const std::string& time_str = (TimeUtil::ToString(p_info.start_time()));

    const int node_id = experiment_tracker_.GetNodeIDFromHostname(experiment_run_id_, hostname);

    int process_id = database_->InsertValuesUnique(
        "Hardware.Process",
        {"NodeID", "pID", "WorkingDirectory", "ProcessName", "Args", "StartTime"},
        {std::to_string(node_id), pid, cwd, name, args, time_str},
        {"NodeID", "pID", "StartTime"});

    process_id_cache_.emplace(
        std::make_pair(GetProcessKey(hostname, p_info.pid(), time_str), process_id));
}

void SystemEventProtoHandler::ProcessDeviceInfo(const SystemEvent::DeviceInfo& d_info, int node_id)
{
    for(const auto& gpu_descriptor : d_info.gpus()) {
        ProcessGPUDescriptor(gpu_descriptor, d_info.hostname(), node_id);
    }
}

void SystemEventProtoHandler::ProcessGPUDescriptor(const SystemEvent::GPUDescriptor& gpu_info,
                                                   const std::string& hostname,
                                                   int node_id)
{
    const auto& name = gpu_info.name();

    int gpu_id = database_->InsertValuesUnique(
            "Hardware.GPU",
            {"NodeID", "Name"},
            {std::to_string(node_id), name},
            {"NodeID", "Name"});
    gpu_id_cache_.emplace(GetGPUKey(hostname, name), gpu_id);
}

std::string SystemEventProtoHandler::GetInterfaceKey(const std::string& hostname,
                                                     const std::string& if_name) const
{
    return hostname + "/" + if_name;
}

std::string SystemEventProtoHandler::GetFileSystemKey(const std::string& hostname,
                                                      const std::string& fs_name) const
{
    return hostname + "/" + fs_name;
}

std::string SystemEventProtoHandler::GetProcessKey(const std::string& hostname,
                                                   int pid,
                                                   const std::string& starttime) const
{
    return hostname + "/" + std::to_string(pid) + "_" + starttime;
}

std::string SystemEventProtoHandler::GetGPUKey(const std::string& hostname,
                                                   const std::string& gpu_name) const
{
    return hostname + "/" + gpu_name;
}
