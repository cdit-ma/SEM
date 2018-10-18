/* logan
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "logan"
 *
 * "logan" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "logan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <vector>
#include <string>
#include <stdint.h>
#include <set>
#include <mutex>
#include <unordered_map>
#include <memory>

namespace SystemEvent{
    class InfoEvent;
    class StatusEvent;
};

class SystemInfo{
    public:
    enum class ProcessState{
        ERROR = 0,
        RUNNING = 1,
        SLEEPING = 2,
        IDLE = 3,
        STOPPED = 4,
        ZOMBIE = 5,
        DEAD = 6
    };

    enum class FileSystemType{
        UNKNOWN = 0,
        LOCAL_DISK = 1,
        NETWORK = 2,
        RAM_DISK = 3,
        CDROM = 4,
        SWAP = 5
    };

    enum class InterfaceState{
        UP = 0,
        RUNNING = 1,
        LOOPBACK = 2,
    };

    virtual ~SystemInfo(){};

    virtual std::chrono::milliseconds get_update_timestamp() const = 0;

    virtual std::string get_hostname() const = 0;

    virtual std::string get_os_arch() const = 0;
    virtual std::string get_os_description() const = 0;
    virtual std::string get_os_name() const = 0;
    virtual std::string get_os_vendor() const = 0;
    virtual std::string get_os_vendor_name() const = 0;
    virtual std::string get_os_vendor_version() const = 0;
    virtual std::string get_os_version() const = 0;


    virtual int get_fs_count() const = 0;
    virtual std::string get_fs_name(const int fs_index) const = 0;
    virtual SystemInfo::FileSystemType get_fs_type(const int fs_index) const = 0;
    
    virtual int get_fs_size(const int fs_index) const = 0;
    virtual int get_fs_free(const int fs_index) const = 0;
    virtual int get_fs_used(const int fs_index) const = 0;
    virtual double get_fs_utilization(const int fs_index) const = 0;


    virtual int get_cpu_count() const = 0;
    virtual int get_cpu_frequency() const = 0;
    
    virtual double get_cpu_utilization(const int cpu_index) const = 0;
    virtual double get_cpu_overall_utilization() const = 0;

    virtual std::string get_cpu_model() const = 0;
    virtual std::string get_cpu_vendor() const = 0;

    //in MB 
    virtual int get_phys_mem() const = 0;
    virtual int get_phys_mem_reserved() const = 0;
    virtual int get_phys_mem_free() const = 0;
    virtual double get_phys_mem_utilization() const = 0;


    virtual int get_interface_count() const = 0;
    virtual std::string get_interface_name(const int interface_index) const = 0;
    virtual std::string get_interface_type(const int interface_index) const = 0;
    virtual std::string get_interface_description(const int interface_index) const = 0;
    
    virtual std::string get_interface_ipv4(const int interface_index) const = 0;
    virtual std::string get_interface_ipv6(const int interface_index) const = 0;
    virtual std::string get_interface_mac(const int interface_index) const = 0;
    virtual int64_t get_interface_speed(const int interface_index) const = 0;
    virtual bool get_interface_state(const int interface_index, SystemInfo::InterfaceState state) const = 0;

    virtual int64_t get_interface_rx_packets(const int interface_index) const = 0;
    virtual int64_t get_interface_rx_bytes(const int interface_index) const = 0;
    virtual int64_t get_interface_tx_packets(const int interface_index) const = 0;
    virtual int64_t get_interface_tx_bytes(const int interface_index) const = 0;
    

    virtual std::set<int> get_process_pids() const = 0;
    
    virtual std::string get_process_name(const int pid) const = 0;
    virtual std::string get_process_arguments(const int pid) const = 0;
    virtual SystemInfo::ProcessState get_process_state(const int pid) const = 0;

    virtual void monitor_processes(const std::string& processName) = 0;
    virtual void ignore_processes(const std::string& processName) = 0;
    
    virtual void monitor_process(const int pid) = 0;
    virtual void ignore_process(const int pid) = 0;
    
    virtual std::set<int> get_monitored_pids() const = 0;

    virtual void ignore_processes() = 0;
    
    virtual std::vector<std::string> get_monitored_processes_names() const = 0;


    virtual int get_monitored_process_cpu(const int pid) const = 0;
    virtual double get_monitored_process_cpu_utilization(const int pid) const = 0;
    virtual int get_monitored_process_phys_mem_used(const int pid) const = 0;
    virtual double get_monitored_process_phys_mem_utilization(const int pid) const = 0;
    virtual int get_monitored_process_thread_count(const int pid) const = 0;
    virtual time_t get_monitored_process_start_time(const int pid) const = 0;
    virtual time_t get_monitored_process_total_time(const int pid) const = 0;
    virtual std::chrono::milliseconds get_monitored_process_update_time(const int pid) const = 0;

    virtual long long get_monitored_process_disk_written(const int pid) const = 0;
    virtual long long get_monitored_process_disk_read(const int pid) const = 0;
    virtual long long get_monitored_process_disk_total(const int pid) const = 0;



    //Refresh
    std::unique_ptr<SystemEvent::StatusEvent> GetSystemStatus(const int listener_id);
    std::unique_ptr<SystemEvent::InfoEvent> GetSystemInfo(const int listener_id);
    void Update();
    int RegisterListener();
private:
    std::mutex mutex_;
    //don't send onetime info for any contained pids
    std::unordered_map<int, std::chrono::milliseconds> pid_updated_times_;
    
    int listener_count_ = 0;

    //listener ID to PIDs Seen
    std::unordered_map<int, std::set<int> > pid_lookups_;

    //listener ID to Count Seen
    std::unordered_map<int, int> listener_message_count_;

    //listener ID to last updated time
    std::unordered_map<int, std::chrono::milliseconds> listener_updated_times_;
protected:
    virtual std::chrono::milliseconds UpdateData() = 0;
};
#endif // SYSTEMINFO_H
