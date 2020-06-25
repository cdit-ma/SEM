#ifndef SIGARSYSTEMINFO_H
#define SIGARSYSTEMINFO_H

#include <stdint.h>
#include <time.h>
#include <chrono>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>

#include "systeminfo.h"

extern "C"{ 
    #include "sigar.h"
    #include "sigar_format.h"
    #include "sigar_ptql.h"
}

SystemInfo& GetSystemInfo();

class SigarSystemInfo: public SystemInfo{
    public:
        SigarSystemInfo();
        ~SigarSystemInfo();

        std::chrono::milliseconds get_update_timestamp() const;

        std::string get_hostname() const;

        int get_cpu_count() const;
        int get_cpu_frequency() const;
        std::string get_cpu_model() const;
        std::string get_cpu_vendor() const;
        
        double get_cpu_utilization(const int cpu_index) const;
        double get_cpu_overall_utilization() const;

        //in MB 
        uint64_t get_phys_mem_kB() const;
        uint64_t get_phys_mem_reserved_kB() const;
        uint64_t get_phys_mem_free_kB() const;

        double get_phys_mem_utilization() const;

        int get_interface_count() const;
        std::string get_interface_name(const int interface_index) const;
        std::string get_interface_type(const int interface_index) const;
        std::string get_interface_description(const int interface_index) const;
        
        std::string get_interface_ipv4(const int interface_index) const;
        std::string get_interface_ipv6(const int interface_index) const;
        std::string get_interface_mac(const int interface_index) const;
        bool get_interface_state(const int interface_index, SystemInfo::InterfaceState state) const;

        uint64_t get_interface_rx_packets(const int interface_index) const;
        uint64_t get_interface_rx_bytes(const int interface_index) const;
        uint64_t get_interface_tx_packets(const int interface_index) const;
        uint64_t get_interface_tx_bytes(const int interface_index) const;
        uint64_t get_interface_speed(const int interface_index) const;
        

        std::set<int> get_process_pids() const;
        SystemInfo::ProcessState get_process_state(const int pid) const;

        std::string get_process_name(const int pid) const;
        std::string get_process_full_name(const int pid) const;
        std::string get_process_arguments(const int pid) const;
        std::string get_process_cwd(const int pid) const;
        
        void monitor_process(const int pid);
        void ignore_process(const int pid);
        std::set<int> get_monitored_pids() const;


        void monitor_processes(const std::string& processName);
        void ignore_processes(const std::string& processName);
        void clear_monitored_processes();
        const std::set<std::string>& get_monitored_processes_names() const;

        int get_monitored_process_cpu(const int pid) const;
        double get_monitored_process_cpu_utilization(const int pid) const;
        uint64_t get_monitored_process_phys_mem_used_kB(const int pid) const;
        double get_monitored_process_phys_mem_utilization(const int pid) const;
        int get_monitored_process_thread_count(const int pid) const;

        std::chrono::milliseconds get_monitored_process_start_time(const int pid) const;
        std::chrono::milliseconds get_monitored_process_cpu_time(const int pid) const;
        std::chrono::milliseconds get_monitored_process_update_time(const int pid) const;

        uint64_t get_monitored_process_disk_written_kB(const int pid) const;
        uint64_t get_monitored_process_disk_read_kB(const int pid) const;
        uint64_t get_monitored_process_disk_total_kB(const int pid) const;

        std::string get_os_arch() const;
        std::string get_os_description() const;
        std::string get_os_name() const;
        std::string get_os_vendor() const;
        std::string get_os_vendor_name() const;
        std::string get_os_vendor_version() const;
        std::string get_os_version() const;


        int get_fs_count() const;
        std::string get_fs_name(const int fs_index) const;
        SystemInfo::FileSystemType get_fs_type(const int fs_index) const;
        
        
        uint64_t get_fs_size_kB(const int fs_index) const;
        uint64_t get_fs_free_kB(const int fs_index) const;
        uint64_t get_fs_used_kB(const int fs_index) const;

        double get_fs_utilization(const int fs_index) const;



        //Refresh
        std::chrono::milliseconds UpdateData();

    private:
        bool open_sigar();
        bool close_sigar();
        sigar_t *sigar_ = 0;

        void update_timestamp();
        bool update_phys_mem(sigar_mem_t* mem);
        bool update_cpu();
        bool update_cpu_list(sigar_cpu_list_t* cpu_list);
        bool update_cpu_info_list(sigar_cpu_info_list_t* cpu_info_list);
        bool update_interfaces();
        bool update_filesystems();
        bool onetime_update_sys_info();
        bool update_processes();

        

        std::chrono::milliseconds get_current_time() const;



        struct CPU{
            sigar_cpu_t cpu;
            sigar_cpu_info_t info;
            sigar_cpu_perc_t usage;
        };

        struct Interface{
            char* name;
            sigar_net_interface_stat_t stats;
            sigar_net_interface_config_t config;
        };

        struct FileSystem{
            sigar_file_system_t system;
            sigar_file_system_usage_t usage;
        };

        struct Process{
            std::string proc_name;
            int pid;
            sigar_proc_exe_t exe;
            sigar_proc_args_t args;
            sigar_proc_state_t state;
            sigar_proc_mem_t mem;
            sigar_proc_cpu_t cpu;
            sigar_proc_disk_io_t disk;
            
            std::chrono::milliseconds lastUpdated_;
        };

        Process& get_process(const int pid) const;
        bool got_process(const int pid) const;

        std::chrono::milliseconds lastUpdate_;
        
 
        sigar_mem_t phys_mem_;
        sigar_sys_info_t sys;
        sigar_net_info_t net;
        
        std::vector<FileSystem> filesystems_;
        std::vector<CPU> cpus_;
        std::vector<Interface> interfaces_;



        std::unordered_map<int, std::unique_ptr<Process> > processes_;
        std::set<int> current_pids_;
        std::set<int> tracked_pids_;
        std::set<std::string> tracked_process_names_;
        bool force_process_name_check_ = false;
        int update_count_ = 0;

        bool stringInString(const std::string& haystack, const std::string& needle) const;

};

#endif //SIGARSYSTEMINFO_H
