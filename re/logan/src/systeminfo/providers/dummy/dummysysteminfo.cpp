//
// Created by mitchell on 23/6/20.
//

#include "dummysysteminfo.h"

SystemInfo& GetSystemInfo()
{
    static DummySystemInfo system_info;
    return system_info;
}

std::chrono::milliseconds DummySystemInfo::get_update_timestamp() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_hostname() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_os_arch() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_os_description() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_os_name() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_os_vendor() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_os_vendor_name() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_os_vendor_version() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_os_version() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
int DummySystemInfo::get_fs_count() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_fs_name(const int fs_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
SystemInfo::FileSystemType DummySystemInfo::get_fs_type(const int fs_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_fs_size_kB(const int fs_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_fs_free_kB(const int fs_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_fs_used_kB(const int fs_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
double DummySystemInfo::get_fs_utilization(const int fs_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
int DummySystemInfo::get_cpu_count() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
int DummySystemInfo::get_cpu_frequency() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
double DummySystemInfo::get_cpu_utilization(const int cpu_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
double DummySystemInfo::get_cpu_overall_utilization() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_cpu_model() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_cpu_vendor() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_phys_mem_kB() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_phys_mem_reserved_kB() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_phys_mem_free_kB() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
double DummySystemInfo::get_phys_mem_utilization() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
int DummySystemInfo::get_interface_count() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_interface_name(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_interface_type(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_interface_description(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_interface_ipv4(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_interface_ipv6(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_interface_mac(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_interface_speed(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
bool DummySystemInfo::get_interface_state(const int interface_index,
                                          SystemInfo::InterfaceState state) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_interface_rx_packets(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_interface_rx_bytes(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_interface_tx_packets(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_interface_tx_bytes(const int interface_index) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::set<int> DummySystemInfo::get_process_pids() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_process_name(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_process_full_name(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_process_arguments(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::string DummySystemInfo::get_process_cwd(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
SystemInfo::ProcessState DummySystemInfo::get_process_state(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
void DummySystemInfo::monitor_processes(const std::string& processName) {}
void DummySystemInfo::ignore_processes(const std::string& processName) {}
void DummySystemInfo::monitor_process(const int pid) {}
void DummySystemInfo::ignore_process(const int pid) {}
std::set<int> DummySystemInfo::get_monitored_pids() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
void DummySystemInfo::clear_monitored_processes() {}
const std::set<std::string>& DummySystemInfo::get_monitored_processes_names() const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
int DummySystemInfo::get_monitored_process_cpu(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
double DummySystemInfo::get_monitored_process_cpu_utilization(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_monitored_process_phys_mem_used_kB(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
double DummySystemInfo::get_monitored_process_phys_mem_utilization(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
int DummySystemInfo::get_monitored_process_thread_count(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::chrono::milliseconds DummySystemInfo::get_monitored_process_start_time(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::chrono::milliseconds DummySystemInfo::get_monitored_process_cpu_time(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::chrono::milliseconds DummySystemInfo::get_monitored_process_update_time(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_monitored_process_disk_written_kB(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_monitored_process_disk_read_kB(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
uint64_t DummySystemInfo::get_monitored_process_disk_total_kB(const int pid) const
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
std::chrono::milliseconds DummySystemInfo::UpdateData()
{
    throw std::logic_error("SIGAR NOT INSTALLED");
}
