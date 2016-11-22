#include "sigarsysteminfo.h"
#include "systeminfo.h"
#include <ctime>
#include <iostream>
#include <set>
#include <algorithm>

SigarSystemInfo::SigarSystemInfo(){
    sigar = 0;
    force_process_name_check = false;
    open_sigar();
    
    initial_update();
}

SigarSystemInfo::~SigarSystemInfo(){
    close_sigar();
}

bool SigarSystemInfo::open_sigar(){
    if(sigar_open(&sigar) == SIGAR_OK){
        return true;
    }else{
        sigar = 0;
        return false;
    }
}

bool SigarSystemInfo::close_sigar(){
    if(sigar_close(sigar) == SIGAR_OK){
        return true;
    }else{
        sigar = 0;
        return false;
    }
}

double SigarSystemInfo::get_update_timestamp() const{
    return get_timestamp(lastUpdate_);
}

double SigarSystemInfo::get_timestamp(const std::chrono::milliseconds t) const{
    return t.count() / 1000;
}

std::string SigarSystemInfo::get_hostname() const{
    return std::string(net.host_name);
}

int SigarSystemInfo::get_cpu_count() const{
    return cpus_.size();
}

int SigarSystemInfo::get_cpu_frequency() const{
    if(get_cpu_count() > 0){
        return cpus_[0].info.mhz;
    }
    return -1;
}

std::string SigarSystemInfo::get_cpu_model() const{
    if(get_cpu_count() > 0){
        return std::string(cpus_[0].info.model);
    }
    return std::string();
}

std::string SigarSystemInfo::get_cpu_vendor() const{
    if(get_cpu_count() > 0){
        return cpus_[0].info.vendor;
    }
    return std::string();
}

double SigarSystemInfo::get_cpu_utilization(const int cpu_index) const{
    if(cpu_index < get_cpu_count()){
        return cpus_[cpu_index].usage.combined;
    }
    return -1;
}

double SigarSystemInfo::get_cpu_overall_utilization() const{
    double length = get_cpu_count();
    if(length > 0){
        double total = 0;
    
        for(CPU cpu: cpus_){
            total += cpu.usage.combined;
        }

        total /= length;
        return total;
    }
    return -1;
}


int SigarSystemInfo::get_phys_mem() const{
    return phys_mem_.total / (1024 * 1024);
}

int SigarSystemInfo::get_phys_mem_reserved() const{
    return phys_mem_.actual_used / (1024 * 1024);
}

int SigarSystemInfo::get_phys_mem_free() const{
    return phys_mem_.free / (1024 * 1024);
}

double SigarSystemInfo::get_phys_mem_utilization() const{
    return phys_mem_.used_percent/100;
}


bool SigarSystemInfo::update_cpu_list(sigar_cpu_list_t * cpu_list){
    return sigar && (sigar_cpu_list_get(sigar, cpu_list) == SIGAR_OK);
}

bool SigarSystemInfo::update_cpu_info_list(sigar_cpu_info_list_t* cpu_info_list){
    return sigar && (sigar_cpu_info_list_get(sigar, cpu_info_list) == SIGAR_OK);
}

bool SigarSystemInfo::update_phys_mem(sigar_mem_t* mem){
    return sigar && (sigar_mem_get(sigar, mem) == SIGAR_OK);
}

bool SigarSystemInfo::update_cpu(){
    sigar_cpu_list_t cpu_list = sigar_cpu_list_t();

    if(!update_cpu_list(&cpu_list)){
        std::cout << "1" << std::endl;
        //Failed to Update
        return false;
    }

    //Setup our vector
    if(cpus_.size() == 0){
        sigar_cpu_info_list_t cpu_info_list = sigar_cpu_info_list_t();

        //Get the info only once
        if(!update_cpu_info_list(&cpu_info_list)){
            std::cout << "2" << std::endl;
            return false;
        }

        //Different number of info
        if(cpu_info_list.number != cpu_list.number){
            std::cout << "3" << std::endl;
            return false;
        }
        
        //Fill the CPU Vector
        cpus_.resize(cpu_list.number);
        for(int i = 0; i < cpu_list.number; i++){
            //Fill in the data
            cpus_[i].cpu = cpu_list.data[i];
            cpus_[i].info = cpu_info_list.data[i];
        }

        //Free the memory?
        sigar_cpu_info_list_destroy(sigar, &cpu_info_list);
    }

    CPU cpu;
    for(int i = 0; i < cpu_list.number; i++){
        cpu = cpus_[i];
        
        sigar_cpu_t current_cpu = cpu_list.data[i];
        
        sigar_cpu_perc_calculate(&cpu.cpu, &current_cpu, &cpu.usage);

        //Update the current cpu
        cpu.cpu = current_cpu;

        cpus_[i] = cpu;
    }

    //Free the memory
    sigar_cpu_list_destroy(sigar, &cpu_list);
	

    return true;
}

bool SigarSystemInfo::initial_update(){
    update_timestamp();
    bool okay = true;
    okay &= update();
    okay &= onetime_update_sys_info();
    return okay;
}
bool SigarSystemInfo::update(){
    std::chrono::milliseconds currentTime = get_current_time();
    auto difference = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdate_);

    //don't update more than 5 times a second
    if(difference.count() < 100){
        return false;
    }
    
    update_timestamp();
    bool okay = true;
    okay &= update_cpu();
    okay &= update_phys_mem(&phys_mem_);
    okay &= update_interfaces();
    okay &= update_processes();
    okay &= update_filesystems();
    return okay;
}

void SigarSystemInfo::update_timestamp(){
    lastUpdate_ = get_current_time();
}

 std::chrono::milliseconds SigarSystemInfo::get_current_time() const{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

bool SigarSystemInfo::update_filesystems(){
    sigar_file_system_list_t fs_list = sigar_file_system_list_t();

    //Get the current list of filesystems
    if(!sigar || (sigar_file_system_list_get(sigar, &fs_list) != SIGAR_OK)){
        return false; 
    }



    //Resize our vector if necessary
    filesystems_.resize(fs_list.number);

    int validCount = 0;

    FileSystem fs;
    for(int i = 0; i < fs_list.number; i++){
        fs.system = fs_list.data[i];

        switch(fs.system.type){
            case SIGAR_FSTYPE_LOCAL_DISK:
            case SIGAR_FSTYPE_NETWORK:
            case SIGAR_FSTYPE_RAM_DISK:
            case SIGAR_FSTYPE_CDROM:
            case SIGAR_FSTYPE_SWAP:
                break;                
            default:
                //Ignore
                continue;
        }
        validCount ++;
        //Get the latest usage
        if(!sigar || (sigar_file_system_usage_get(sigar, fs.system.dir_name, &fs.usage) != SIGAR_OK)){
            return false;
        }

        //Update our filesystem list
        filesystems_[validCount] = fs;
    }
    filesystems_.resize(validCount);

    sigar_file_system_list_destroy(sigar, &fs_list);
    return true;
}

bool SigarSystemInfo::update_interfaces(){
    
    sigar_net_interface_list_t interface_list = sigar_net_interface_list_t();
    
    //Get the current list of network interfaces
    if(!sigar || (sigar_net_interface_list_get(sigar, &interface_list) != SIGAR_OK)){
        return false; 
    }

    //Resize our vector if necessary
    if(interfaces_.size() != interface_list.number){
        interfaces_.resize(interface_list.number);
    }


    Interface interface;
    for(int i = 0; i < interface_list.number; i++){
        interface.name = interface_list.data[i];

        //Get the latest stats
        if(!sigar || (sigar_net_interface_stat_get(sigar, interface.name, &interface.stats) != SIGAR_OK)){
            return false;
        }

        //Get the latest config
        if(!sigar || (sigar_net_interface_config_get(sigar, interface.name, &interface.config) != SIGAR_OK)){
            return false;
        }

        //Update our interface list
        interfaces_[i] = interface;
    }
    sigar_net_interface_list_destroy(sigar, &interface_list);
    return true;
}

int SigarSystemInfo::get_interface_count() const{
    return interfaces_.size();
}

std::string SigarSystemInfo::get_interface_name(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].config.name;
    }
    return std::string();
}

std::string SigarSystemInfo::get_interface_type(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].config.type;
    }
    return std::string();
}

bool SigarSystemInfo::get_interface_state(const int interface_index, SystemInfo::InterfaceState state) const{
    if(interface_index < get_interface_count()){
        int interfaceFlags = interfaces_[interface_index].config.flags;

        switch(state){
            case SystemInfo::InterfaceState::LOOPBACK:{
                return interfaceFlags & SIGAR_IFF_LOOPBACK;
            }
            case SystemInfo::InterfaceState::UP:{
                return interfaceFlags & SIGAR_IFF_UP;
            }
            case SystemInfo::InterfaceState::RUNNING:{
                return interfaceFlags & SIGAR_IFF_RUNNING;
            }
            default:{
                break;
            }
        }
    }
    return false;
}

int64_t SigarSystemInfo::get_interface_rx_packets(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.rx_packets;
    }
    return -1;
}

int64_t SigarSystemInfo::get_interface_rx_bytes(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.rx_bytes;
    }
    return -1;
}

int64_t SigarSystemInfo::get_interface_tx_packets(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.tx_packets;
    }
    return -1;
}

int64_t SigarSystemInfo::get_interface_tx_bytes(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.tx_bytes;
    }
    return -1;
}

int64_t SigarSystemInfo::get_interface_speed(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.speed;
    }
    return -1;
}


std::string SigarSystemInfo::get_interface_description(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return std::string(interfaces_[interface_index].config.description);
    }
    return std::string();
}

std::string SigarSystemInfo::get_interface_ipv4(const int interface_index) const{
    if(interface_index < get_interface_count()){
        //XXX.XXX.XXX.XXX
        //Max Length: 15 + 1
        char addr_str[16];
        sigar_net_address_t  adt = interfaces_[interface_index].config.address;
        if(sigar_net_address_to_string(sigar, &adt , addr_str) == SIGAR_OK){
            return std::string(addr_str);
        }
    }
    return std::string();
}

std::string SigarSystemInfo::get_interface_ipv6(const int interface_index) const{
    if(interface_index < get_interface_count()){
        //ABCD:ABCD:ABCD:ABCD:ABCD:ABCD:192.168.158.190
        //Max Length: 45 + 1
        
        char addr_str[46];
        sigar_net_address_t  adt = interfaces_[interface_index].config.address6;
        
        if(sigar_net_address_to_string(sigar, &adt , addr_str) == SIGAR_OK){
            return std::string(addr_str);
        }
    }
    return std::string();
}


std::string SigarSystemInfo::get_interface_mac(const int interface_index) const{
    if(interface_index < get_interface_count()){
        //00:00:00:00:00:00
        char addr_str[16];
        sigar_net_address_t  adt = interfaces_[interface_index].config.hwaddr;
        if(sigar_net_address_to_string(sigar, &adt , addr_str) == SIGAR_OK){
            return std::string(addr_str);
        }
    }
    return std::string();
}



std::string SigarSystemInfo::get_os_arch() const{
    return std::string(sys.arch);
    
}
std::string SigarSystemInfo::get_os_description() const{
    return std::string(sys.description);
}
std::string SigarSystemInfo::get_os_name() const{
    return std::string(sys.name);
}
std::string SigarSystemInfo::get_os_vendor() const{
    return std::string(sys.vendor);
}
std::string SigarSystemInfo::get_os_vendor_name() const{
    return std::string(sys.vendor_name);
}
std::string SigarSystemInfo::get_os_vendor_version() const{
    return std::string(sys.vendor_version);
}
std::string SigarSystemInfo::get_os_version() const{
    return std::string(sys.version);
}

bool SigarSystemInfo::onetime_update_sys_info(){
    if(sigar_sys_info_get(sigar, &sys) != SIGAR_OK){
        return false;
    }
    if(sigar_net_info_get(sigar, &net) != SIGAR_OK){
        return false;
    }

    return true;
}

int SigarSystemInfo::get_fs_count() const{
    return filesystems_.size();
}

std::string SigarSystemInfo::get_fs_name(const int fs_index) const{
    if(fs_index < get_fs_count()){
        return std::string(filesystems_[fs_index].system.dir_name);
    }
    return std::string();
}

SystemInfo::FileSystemType SigarSystemInfo::get_fs_type(const int fs_index) const{
    if(fs_index < get_fs_count()){
        switch(filesystems_[fs_index].system.type){
            case SIGAR_FSTYPE_LOCAL_DISK:
                return SystemInfo::FileSystemType::LOCAL_DISK;
            case SIGAR_FSTYPE_NETWORK:
                return SystemInfo::FileSystemType::NETWORK;
            case SIGAR_FSTYPE_RAM_DISK:
                return SystemInfo::FileSystemType::RAM_DISK;
            case SIGAR_FSTYPE_CDROM:
                return SystemInfo::FileSystemType::CDROM;
            case SIGAR_FSTYPE_SWAP:
                return SystemInfo::FileSystemType::SWAP;
            default:
                break;
        }
    }
    
    return SystemInfo::FileSystemType::UNKNOWN;
}
        
int SigarSystemInfo::get_fs_size(const int fs_index) const{
    if(fs_index < get_fs_count()){
        return filesystems_[fs_index].usage.total / 1024;
    }
    return -1;
}
int SigarSystemInfo::get_fs_free(const int fs_index) const{
    if(fs_index < get_fs_count()){
        return filesystems_[fs_index].usage.avail / 1024;
    }
    return -1;
}

int SigarSystemInfo::get_fs_used(const int fs_index) const{
    if(fs_index < get_fs_count()){
        return filesystems_[fs_index].usage.used / 1024;
    }
    return -1;
}

double SigarSystemInfo::get_fs_utilization(const int fs_index) const{
    if(fs_index < get_fs_count()){
        double used = filesystems_[fs_index].usage.used;
        double total = filesystems_[fs_index].usage.total;
        if(total > 0){
            return used / total;
        }
    }
    return -1;
}


std::vector<int> SigarSystemInfo::get_process_pids() const{
    std::vector<int> out;
    for(auto pid : processes_){
        out.push_back(pid.first);
    }
    return out;
}

std::string SigarSystemInfo::get_process_name(const int pid) const{
    if(processes_.count(pid)){
        return std::string(processes_.at(pid)->exe.name);    
    }
    return std::string();
}

std::string SigarSystemInfo::get_process_arguments(const int pid) const{
    if(processes_.count(pid)){
        std::string out = "";
    
        sigar_proc_args_t args = processes_.at(pid)->args;

        for(size_t i = 0; i < args.number; i++){
            std::string temp(args.data[i]);
            if(i == args.number-1){
                out = out + temp;
            } else {
                out = out + temp + " ";
            }
        }
        return out;
    }
    return std::string();
}

SystemInfo::ProcessState SigarSystemInfo::get_process_state(const int pid) const{
    if(processes_.count(pid)){
        switch(processes_.at(pid)->state.state){
            case 'D':
                return SystemInfo::ProcessState::DISK_SLEEP;
            case 'R':
                return SystemInfo::ProcessState::RUNNING;
            case 'S':
                return SystemInfo::ProcessState::SLEEPING;
            case 'T':
                return SystemInfo::ProcessState::STOPPED;
            case 'Z':
                return SystemInfo::ProcessState::ZOMBIE;
            default:
                break;  
        }
    }
    return SystemInfo::ProcessState::ERROR;
}

//get cpu index 'pid'' is currently running on
int SigarSystemInfo::get_monitored_process_cpu(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->state.processor;    
    }
    return -1;
}

double SigarSystemInfo::get_monitored_process_cpu_utilization(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->cpu.percent;
    }
    return -1;
}

int SigarSystemInfo::get_monitored_process_phys_mem_used(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->mem.resident;
    }
    return -1;
}

double SigarSystemInfo::get_monitored_process_phys_mem_utilization(const int pid) const{
    if(processes_.count(pid)){
        double mem = double(processes_.at(pid)->mem.resident) / 1024 / 1024;
        double total = double(get_phys_mem());
        if(total > 0){
            return mem / total;
        }
    }
    return -1;
}

int SigarSystemInfo::get_monitored_process_thread_count(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->state.threads;
    }
    return -1;
}

time_t SigarSystemInfo::get_monitored_process_start_time(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->cpu.start_time/1000;
    }
    return -1;
}

time_t SigarSystemInfo::get_monitored_process_total_time(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->cpu.total;
    }
    return -1;
}


long long SigarSystemInfo::get_monitored_process_disk_written(const int pid) const{
    if(!processes_.count(pid)){
        return -1;
    }

    return processes_.at(pid)->disk.bytes_written;
}
long long SigarSystemInfo::get_monitored_process_disk_read(const int pid) const{
    if(!processes_.count(pid)){
        return -1;
    }

    return processes_.at(pid)->disk.bytes_read;
}
long long SigarSystemInfo::get_monitored_process_disk_total(const int pid) const{
    if(!processes_.count(pid)){
        return -1;
    }

    return processes_.at(pid)->disk.bytes_total;
}
bool SigarSystemInfo::update_processes(){

    sigar_proc_list_t process_list = sigar_proc_list_t();

    if(sigar_proc_list_get(sigar, &process_list) != SIGAR_OK){
        return false;
    }

    current_pids_.clear();

    Process* process = 0;

    std::chrono::milliseconds t = get_current_time();
    for(size_t i = 0; i < process_list.number; i++){
        int pid = process_list.data[i];
        current_pids_.insert(pid);

        bool seenPIDBefore = processes_.count(pid); 
        
        if(seenPIDBefore){
            process = processes_[pid];
        }else{
            process = new Process();
        }

        //Require State
        if(sigar_proc_state_get(sigar, process_list.data[i], &process->state) != SIGAR_OK){
            continue;
        }
        
        std::string slash = "/";
        //Cross Slash Detection.
        #ifdef _WIN32
            slash = "\\";
        #endif
        
        if(!seenPIDBefore){
            //we dont have any records of this process yet.
            //Add process name and args to struct
            if(sigar_proc_exe_get(sigar, process_list.data[i], &process->exe) != SIGAR_OK){
                //continue;
            }

            if(sigar_proc_args_get(sigar, process_list.data[i], &process->args) != SIGAR_OK){
                //continue;
            }

            //Trim the Path out of the name
            std::string procName = std::string(process->exe.name);
			procName = procName.substr(procName.find_last_of(slash) + 1, std::string::npos);
            process->proc_name = procName;
            
        }

        if(force_process_name_check){
            for(std::string query: tracked_process_names_){
                if(stringInString(process->proc_name, query)){
                    //Monitor the process
                    monitor_process(pid);
                    break;
                }
            }
        }

        processes_[pid] = process;

        //If we care about tracking this PID
        if(tracked_pids_.count(pid)){
            auto difference = std::chrono::duration_cast<std::chrono::seconds>(t - process->lastUpdated_);
            
            //If we have this pid already and more than 1 second has elapsed
            if(difference.count() >= 1){
                if(sigar_proc_cpu_get(sigar, process_list.data[i], &process->cpu) != SIGAR_OK){
                //continue;
                }
                if(sigar_proc_mem_get(sigar, process_list.data[i], &process->mem) != SIGAR_OK){
                    //continue;
                }
                if(sigar_proc_disk_io_get(sigar, process_list.data[i], &process->disk) != SIGAR_OK){
                    //continue;
                }
            }
            process->lastUpdated_ = t;
        }

        processes_[pid] = process;
    }

    for(auto it = processes_.begin(); it != processes_.end();){
        int pid = (*it).first;
        if(!current_pids_.count(pid)){
            //Free up the memory
            Process* p = (*it).second;
            it = processes_.erase(it);
            //Remove the pid from the tracked list
            tracked_pids_.erase(pid);
            delete p;
        }else{
            it++;
        }
    }

    sigar_proc_list_destroy(sigar, &process_list);
    
    //Unset our flag
    force_process_name_check = false;
    

    return true;
}

void SigarSystemInfo::monitor_process(const int pid){
    tracked_pids_.insert(pid);
}

void SigarSystemInfo::ignore_process(const int pid){
    tracked_pids_.erase(pid);
}

std::vector<int> SigarSystemInfo::get_monitored_pids() const{
    return std::vector<int>(tracked_pids_.begin(), tracked_pids_.end());
}


void SigarSystemInfo::monitor_processes(const std::string processName){
    //If can't find
    if(std::find(tracked_process_names_.begin(), tracked_process_names_.end(), processName) == tracked_process_names_.end()){
        tracked_process_names_.push_back(processName);
        force_process_name_check = true;
    }
}

void SigarSystemInfo::ignore_processes(const std::string processName){
    //If can't find
    auto location = std::find(tracked_process_names_.begin(), tracked_process_names_.end(), processName);
    if(location != tracked_process_names_.end()){
        tracked_process_names_.erase(location);
    }
}

std::vector<std::string> SigarSystemInfo::get_monitored_processes_names() const{
    return tracked_process_names_;
}

double SigarSystemInfo::get_monitored_process_update_time(const int pid) const{
    if(processes_.count(pid)){
        return get_timestamp(processes_.at(pid)->lastUpdated_);
    }
    return -1;
}
        
 

bool SigarSystemInfo::stringInString(const std::string haystack, const std::string needle) const{
    auto match = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return (match != haystack.end() );   
}