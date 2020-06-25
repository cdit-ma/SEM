//
// Created by mitchell on 23/6/20.
//

#ifndef SEM_DUMMYSYSTEMINFO_H
#define SEM_DUMMYSYSTEMINFO_H

#include "systeminfo.h"

SystemInfo& GetSystemInfo();

class DummySystemInfo : public SystemInfo {
public:
    std::chrono::milliseconds get_update_timestamp() const override;
    std::string get_hostname() const override;
    std::string get_os_arch() const override;
    std::string get_os_description() const override;
    std::string get_os_name() const override;
    std::string get_os_vendor() const override;
    std::string get_os_vendor_name() const override;
    std::string get_os_vendor_version() const override;
    std::string get_os_version() const override;
    int get_fs_count() const override;
    std::string get_fs_name(const int fs_index) const override;
    FileSystemType get_fs_type(const int fs_index) const override;
    uint64_t get_fs_size_kB(const int fs_index) const override;
    uint64_t get_fs_free_kB(const int fs_index) const override;
    uint64_t get_fs_used_kB(const int fs_index) const override;
    double get_fs_utilization(const int fs_index) const override;
    int get_cpu_count() const override;
    int get_cpu_frequency() const override;
    double get_cpu_utilization(const int cpu_index) const override;
    double get_cpu_overall_utilization() const override;
    std::string get_cpu_model() const override;
    std::string get_cpu_vendor() const override;
    uint64_t get_phys_mem_kB() const override;
    uint64_t get_phys_mem_reserved_kB() const override;
    uint64_t get_phys_mem_free_kB() const override;
    double get_phys_mem_utilization() const override;
    int get_interface_count() const override;
    std::string get_interface_name(const int interface_index) const override;
    std::string get_interface_type(const int interface_index) const override;
    std::string get_interface_description(const int interface_index) const override;
    std::string get_interface_ipv4(const int interface_index) const override;
    std::string get_interface_ipv6(const int interface_index) const override;
    std::string get_interface_mac(const int interface_index) const override;
    uint64_t get_interface_speed(const int interface_index) const override;
    bool
    get_interface_state(const int interface_index, SystemInfo::InterfaceState state) const override;
    uint64_t get_interface_rx_packets(const int interface_index) const override;
    uint64_t get_interface_rx_bytes(const int interface_index) const override;
    uint64_t get_interface_tx_packets(const int interface_index) const override;
    uint64_t get_interface_tx_bytes(const int interface_index) const override;
    std::set<int> get_process_pids() const override;
    std::string get_process_name(const int pid) const override;
    std::string get_process_full_name(const int pid) const override;
    std::string get_process_arguments(const int pid) const override;
    std::string get_process_cwd(const int pid) const override;
    ProcessState get_process_state(const int pid) const override;
    void monitor_processes(const std::string& processName) override;
    void ignore_processes(const std::string& processName) override;
    void monitor_process(const int pid) override;
    void ignore_process(const int pid) override;
    std::set<int> get_monitored_pids() const override;
    void clear_monitored_processes() override;
    const std::set<std::string>& get_monitored_processes_names() const override;
    int get_monitored_process_cpu(const int pid) const override;
    double get_monitored_process_cpu_utilization(const int pid) const override;
    uint64_t get_monitored_process_phys_mem_used_kB(const int pid) const override;
    double get_monitored_process_phys_mem_utilization(const int pid) const override;
    int get_monitored_process_thread_count(const int pid) const override;
    std::chrono::milliseconds get_monitored_process_start_time(const int pid) const override;
    std::chrono::milliseconds get_monitored_process_cpu_time(const int pid) const override;
    std::chrono::milliseconds get_monitored_process_update_time(const int pid) const override;
    uint64_t get_monitored_process_disk_written_kB(const int pid) const override;
    uint64_t get_monitored_process_disk_read_kB(const int pid) const override;
    uint64_t get_monitored_process_disk_total_kB(const int pid) const override;

protected:
    std::chrono::milliseconds UpdateData() override;
};

#endif // SEM_DUMMYSYSTEMINFO_H
