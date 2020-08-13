#include "sigarsysteminfo.h"

#include <sigar.h>
#include <sigar_format.h>
#include <ctime>
#include <iostream>
#include <algorithm>

SystemInfo& GetSystemInfo(){
    static SigarSystemInfo system_info;
    return system_info;
}

SigarSystemInfo::SigarSystemInfo(){
    open_sigar();
    update_timestamp();
    onetime_update_sys_info();
    UpdateData();
}

SigarSystemInfo::~SigarSystemInfo(){
    close_sigar();
}

bool SigarSystemInfo::open_sigar(){
    if(sigar_open(&sigar_) == SIGAR_OK){
        return true;
    }else{
        sigar_ = 0;
        return false;
    }
}

bool SigarSystemInfo::close_sigar(){
    processes_.clear();

    if(sigar_close(sigar_) == SIGAR_OK){
        return true;
    }else{
        sigar_ = 0;
        return false;
    }
}

std::chrono::milliseconds SigarSystemInfo::get_update_timestamp() const{
    return lastUpdate_;
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


uint64_t SigarSystemInfo::get_phys_mem_kB() const{
    return phys_mem_.total / 1000;
}

uint64_t SigarSystemInfo::get_phys_mem_reserved_kB() const{
    return phys_mem_.actual_used / 1000;
}

uint64_t SigarSystemInfo::get_phys_mem_free_kB() const{
    return phys_mem_.free / 1000;
}

double SigarSystemInfo::get_phys_mem_utilization() const{
    return phys_mem_.used_percent / 100.0;
}


bool SigarSystemInfo::update_cpu_list(sigar_cpu_list_t * cpu_list){
    return sigar_ && (sigar_cpu_list_get(sigar_, cpu_list) == SIGAR_OK);
}

bool SigarSystemInfo::update_cpu_info_list(sigar_cpu_info_list_t* cpu_info_list){
    return sigar_ && (sigar_cpu_info_list_get(sigar_, cpu_info_list) == SIGAR_OK);
}

bool SigarSystemInfo::update_phys_mem(sigar_mem_t* mem){
    return sigar_ && (sigar_mem_get(sigar_, mem) == SIGAR_OK);
}

bool SigarSystemInfo::update_cpu(){
    sigar_cpu_list_t cpu_list = sigar_cpu_list_t();

    if(!update_cpu_list(&cpu_list)){
        //Failed to Update
        return false;
    }

    //Setup our vector
    if(cpus_.size() == 0){
        sigar_cpu_info_list_t cpu_info_list = sigar_cpu_info_list_t();

        //Get the info only once
        if(!update_cpu_info_list(&cpu_info_list)){
            return false;
        }

        //Different number of info
        if(cpu_info_list.number != cpu_list.number){
            return false;
        }
        
        //Fill the CPU Vector
        cpus_.resize(cpu_list.number);
        for(size_t i = 0; i < cpu_list.number; i++){
            //Fill in the data
            cpus_[i].cpu = cpu_list.data[i];
            cpus_[i].info = cpu_info_list.data[i];
        }

        //Free the memory?
        sigar_cpu_info_list_destroy(sigar_, &cpu_info_list);
    }

    CPU cpu;
    for(size_t i = 0; i < cpu_list.number; i++){
        cpu = cpus_[i];
        
        sigar_cpu_t current_cpu = cpu_list.data[i];
        
        sigar_cpu_perc_calculate(&cpu.cpu, &current_cpu, &cpu.usage);

        //Update the current cpu
        cpu.cpu = current_cpu;

        cpus_[i] = cpu;
    }

    //Free the memory
    sigar_cpu_list_destroy(sigar_, &cpu_list);
    return true;
}

std::chrono::milliseconds SigarSystemInfo::UpdateData(){
    std::chrono::milliseconds current_time = get_current_time();
    auto difference = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - lastUpdate_);

    //don't update more than 10 times a second
    if(difference.count() >= 100 || update_count_ == 0){
        update_timestamp();
        
        update_cpu();
        update_phys_mem(&phys_mem_);
        update_interfaces();
        update_processes();
        update_filesystems();

        update_count_++;
    }
    return lastUpdate_;
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
    if(!sigar_ || (sigar_file_system_list_get(sigar_, &fs_list) != SIGAR_OK)){
        return false; 
    }



    //Resize our vector if necessary
    filesystems_.resize(fs_list.number);

    int validCount = 0;

    FileSystem fs;
    for(size_t i = 0; i < fs_list.number; i++){
        fs.system = fs_list.data[i];

        switch(fs.system.type){
            case SIGAR_FSTYPE_LOCAL_DISK:
            case SIGAR_FSTYPE_NETWORK:
            case SIGAR_FSTYPE_SWAP:
                //Only allow the above types
                break;
            default:
                //Ignore
                continue;
        }
        //Get the latest usage
        if(!sigar_ || (sigar_file_system_usage_get(sigar_, fs.system.dir_name, &fs.usage) != SIGAR_OK)){
            return false;
        }

        //Update our filesystem list
        filesystems_[validCount] = fs;
        validCount ++;
    }

    filesystems_.resize(validCount);

    sigar_file_system_list_destroy(sigar_, &fs_list);
    return true;
}

bool SigarSystemInfo::update_interfaces(){
    
    sigar_net_interface_list_t interface_list = sigar_net_interface_list_t();
    
    //Get the current list of network interfaces
    if(!sigar_ || (sigar_net_interface_list_get(sigar_, &interface_list) != SIGAR_OK)){
        return false; 
    }

    //Resize our vector if necessary
    if(interfaces_.size() != interface_list.number){
        interfaces_.resize(interface_list.number);
    }


    Interface interface;
    for(size_t i = 0; i < interface_list.number; i++){
        interface.name = interface_list.data[i];

        //Get the latest stats
        if(!sigar_ || (sigar_net_interface_stat_get(sigar_, interface.name, &interface.stats) != SIGAR_OK)){
            return false;
        }

        //Get the latest config
        if(!sigar_ || (sigar_net_interface_config_get(sigar_, interface.name, &interface.config) != SIGAR_OK)){
            return false;
        }

        //Update our interface list
        interfaces_[i] = interface;
    }
    sigar_net_interface_list_destroy(sigar_, &interface_list);
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
        int interfaceFlags = (int)(interfaces_[interface_index].config.flags);

        switch(state){
            case SystemInfo::InterfaceState::LOOPBACK:{
                return (interfaceFlags & SIGAR_IFF_LOOPBACK) != 0;
            }
            case SystemInfo::InterfaceState::UP:{
                return (interfaceFlags & SIGAR_IFF_UP) != 0;
            }
            case SystemInfo::InterfaceState::RUNNING:{
                return (interfaceFlags & SIGAR_IFF_RUNNING) != 0;
            }
            default:{
                break;
            }
        }
    }
    return false;
}

uint64_t SigarSystemInfo::get_interface_rx_packets(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.rx_packets;
    }
    return 0;
}

uint64_t SigarSystemInfo::get_interface_rx_bytes(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.rx_bytes;
    }
    return 0;
}

uint64_t SigarSystemInfo::get_interface_tx_packets(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.tx_packets;
    }
    return 0;
}

uint64_t SigarSystemInfo::get_interface_tx_bytes(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.tx_bytes;
    }
    return 0;
}

uint64_t SigarSystemInfo::get_interface_speed(const int interface_index) const{
    if(interface_index < get_interface_count()){
        return interfaces_[interface_index].stats.speed;
    }
    return 0;
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
        if(sigar_net_address_to_string(sigar_, &adt , addr_str) == SIGAR_OK){
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
        
        if(sigar_net_address_to_string(sigar_, &adt , addr_str) == SIGAR_OK){
            return std::string(addr_str);
        }
    }
    return std::string();
}


std::string SigarSystemInfo::get_interface_mac(const int interface_index) const{
    if(interface_index < get_interface_count()){
        //00:00:00:00:00:00
        //Max length: 17 + 1
        char addr_str[18];
        sigar_net_address_t  adt = interfaces_[interface_index].config.hwaddr;
        if(sigar_net_address_to_string(sigar_, &adt , addr_str) == SIGAR_OK){
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
    if(sigar_sys_info_get(sigar_, &sys) != SIGAR_OK){
        return false;
    }
    if(sigar_net_info_get(sigar_, &net) != SIGAR_OK){
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
        
uint64_t SigarSystemInfo::get_fs_size_kB(const int fs_index) const{
    if(fs_index < get_fs_count()){
        return (filesystems_[fs_index].usage.total * 1000) / 1024;
    }
    return 0;
}

uint64_t SigarSystemInfo::get_fs_free_kB(const int fs_index) const{
    if(fs_index < get_fs_count()){
        return (filesystems_[fs_index].usage.avail * 1000) / 1024;
    }
    return 0;
}

uint64_t SigarSystemInfo::get_fs_used_kB(const int fs_index) const{
    if(fs_index < get_fs_count()){
        return (filesystems_[fs_index].usage.used * 1000) / 1024;
    }
    return 0;
}

double SigarSystemInfo::get_fs_utilization(const int fs_index) const{
    if(fs_index < get_fs_count()){
        double used = (double)(filesystems_[fs_index].usage.used);
        double total = (double)(filesystems_[fs_index].usage.total);
        if(total > 0){
            return used / total;
        }
    }
    return -1;
}


std::set<int> SigarSystemInfo::get_process_pids() const{
    std::set<int> out;
    for(const auto& pid : processes_){
        out.emplace(pid.first);
    }
    return out;
}

SigarSystemInfo::Process& SigarSystemInfo::get_process(const int pid) const{
    return *(processes_.at(pid));
}
bool SigarSystemInfo::got_process(const int pid) const{
    return processes_.count(pid);
}

std::string SigarSystemInfo::get_process_name(const int pid) const{
    try{
        return get_process(pid).state.name;
    }catch(const std::exception& ex){
        return std::string();
    }
}

std::string SigarSystemInfo::get_process_full_name(const int pid) const{
    try{
        return get_process(pid).exe.name;
    }catch(const std::exception& ex){
        return std::string();
    }
}

std::string SigarSystemInfo::get_process_cwd(const int pid) const{
    try{
        return get_process(pid).exe.cwd;
    }catch(const std::exception& ex){
        return std::string();
    }
}

std::string SigarSystemInfo::get_process_arguments(const int pid) const{
    try{
        auto& proc = get_process(pid);

        std::string arg_str;

        const auto& args = proc.args;

        for(size_t i = 0; i < args.number; i++){
            arg_str += args.data[i];

            if(i != args.number-1){
                arg_str += " ";
            }
        }
        return arg_str;
        
    }catch(const std::exception& ex){
        return std::string();
    }
}

SystemInfo::ProcessState SigarSystemInfo::get_process_state(const int pid) const{
    try{
        auto& proc = get_process(pid);

        switch(proc.state.state){
            case SIGAR_PROC_STATE_IDLE:
                return SystemInfo::ProcessState::IDLE;
            case SIGAR_PROC_STATE_RUN:
                return SystemInfo::ProcessState::RUNNING;
            case SIGAR_PROC_STATE_SLEEP:
                return SystemInfo::ProcessState::SLEEPING;
            case SIGAR_PROC_STATE_STOP:
                return SystemInfo::ProcessState::STOPPED;
            case SIGAR_PROC_STATE_ZOMBIE:
                return SystemInfo::ProcessState::ZOMBIE;
            default:
                break;  
        }
    }catch(const std::exception& ex){
    }
    return SystemInfo::ProcessState::ERROR;
}

//get cpu index 'pid'' is currently running on
int SigarSystemInfo::get_monitored_process_cpu(const int pid) const{
    try{
        return get_process(pid).state.processor;
    }catch(const std::exception& ex){
        return -1;
    }
}

double SigarSystemInfo::get_monitored_process_cpu_utilization(const int pid) const{
    try{
        return get_process(pid).cpu.percent;
    }catch(const std::exception& ex){
        return 0;
    }
}

uint64_t SigarSystemInfo::get_monitored_process_phys_mem_used_kB(const int pid) const{
    try{
        return get_process(pid).mem.resident / 1000;
    }catch(const std::exception& ex){
        return 0;
    }
}

double SigarSystemInfo::get_monitored_process_phys_mem_utilization(const int pid) const{
    double proc_memory_kB = get_monitored_process_phys_mem_used_kB(pid);
    double total_memory_kB = get_phys_mem_kB();

    if(proc_memory_kB > 0 && total_memory_kB > 0){
        return proc_memory_kB / total_memory_kB;
    }else{
        return 0;
    }
}

int SigarSystemInfo::get_monitored_process_thread_count(const int pid) const{
    try{
        return get_process(pid).state.threads;
    }catch(const std::exception& ex){
        return 0;
    }
}

std::chrono::milliseconds SigarSystemInfo::get_monitored_process_start_time(const int pid) const{
    try{
        return std::chrono::milliseconds(get_process(pid).cpu.start_time);
    }catch(const std::exception& ex){
        return std::chrono::milliseconds(0);
    }
}

std::chrono::milliseconds SigarSystemInfo::get_monitored_process_cpu_time(const int pid) const{
    try{
        auto& process = get_process(pid);
        return std::chrono::milliseconds(get_process(pid).cpu.total);
    }catch(const std::exception& ex){
        return std::chrono::milliseconds(0);
    }
}

std::chrono::milliseconds SigarSystemInfo::get_monitored_process_update_time(const int pid) const{
    try{
        return get_process(pid).lastUpdated_;
    }catch(const std::exception& ex){
        return std::chrono::milliseconds(-1);
    }
}


uint64_t SigarSystemInfo::get_monitored_process_disk_written_kB(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->disk.bytes_written / 1000;
    }
    return 0;
}

uint64_t SigarSystemInfo::get_monitored_process_disk_read_kB(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->disk.bytes_read / 1000;
    }
    return 0;
}

void PrintError(int e){
    std::string error;
    switch(e){
        case SIGAR_OK:{
            error = "SIGAR_OK";
            break;
        }
        case SIGAR_START_ERROR:{
            error = "SIGAR_START_ERROR";
            break;
        }
        case SIGAR_ENOTIMPL:{
            error = "SIGAR_ENOTIMPL";
            break;
        }
        case SIGAR_OS_START_ERROR:{
            error = "SIGAR_OS_START_ERROR";
            break;
        }
        #ifndef _WIN32
        case SIGAR_ENOENT:{
            error = "SIGAR_ENOENT";
            break;
        }
        case SIGAR_EACCES:{
            error = "SIGAR_EACCES";
            break;
        }
        case SIGAR_ENXIO:{
            error = "SIGAR_ENXIO";
            break;
        }
        #endif
    }
    std::cerr << e << " " << error << std::endl;
}

uint64_t SigarSystemInfo::get_monitored_process_disk_total_kB(const int pid) const{
    if(processes_.count(pid)){
        return processes_.at(pid)->disk.bytes_total / 1000;
    }
    return 0;
}

bool SigarSystemInfo::update_processes(){

    sigar_proc_list_t process_list = sigar_proc_list_t();

    if(sigar_proc_list_get(sigar_, &process_list) != SIGAR_OK){
        return false;
    }

    current_pids_.clear();

    auto current_timestamp = get_update_timestamp();
    for(size_t i = 0; i < process_list.number; i++){
        const auto& pid = process_list.data[i];
        const auto seen_pid = got_process(pid);

        if(!seen_pid){
            //Construct a new process
            auto proc_ptr = new Process();
            auto& process = *proc_ptr;
            
            //Set things
            process.pid = pid;
            sigar_proc_exe_get(sigar_, pid, &(process.exe));
            sigar_proc_args_get(sigar_, pid, &(process.args));

            std::string slash = "/";
            //Cross Slash Detection.
            #ifdef _WIN32
                slash = "\\";
            #endif
            //Trim the Path out of the name
            const auto& exe_name = std::string(process.exe.name);
			const auto& process_name = exe_name.substr(exe_name.find_last_of(slash) + 1, std::string::npos);
            
            process.proc_name = process_name;
            processes_.emplace(std::make_pair(pid, std::unique_ptr<Process>(proc_ptr)));
        }
        try{
            auto& process = get_process(pid);
            current_pids_.insert(pid);
            
            auto get_info = seen_pid == false;

            //Check if this item matches
            if(!seen_pid || force_process_name_check_){
                bool matched_process_names = false;
                
                //Check if this process name is in the list of things to track
                for(const auto& query: tracked_process_names_){
                    if(stringInString(process.proc_name, query)){
                        matched_process_names = true;
                        break;
                    }
                }

                if(matched_process_names){
                    monitor_process(pid);
                    get_info = true;
                }else{
                    ignore_process(pid);
                }
            }
        
            if(seen_pid){
                //Don't allow updates more than 1 second for processes
                auto difference = std::chrono::duration_cast<std::chrono::seconds>(current_timestamp - process.lastUpdated_);
                get_info = difference.count() >= 1;
            }

            //If we care about tracking this PID, get it's state cpu mem and disk
            if(get_info && tracked_pids_.count(pid)){
                sigar_proc_state_get(sigar_, pid, &process.state);
                sigar_proc_cpu_get(sigar_, pid, &process.cpu);
                sigar_proc_mem_get(sigar_, pid, &process.mem);
                sigar_proc_disk_io_get(sigar_, pid, &process.disk);
                process.lastUpdated_ = current_timestamp;
            }
        }catch(const std::exception& ex){
            std::cerr << ex.what() << std::endl;
        }
    }

    //Purge deleted processess
    for(auto it = processes_.begin(); it != processes_.end();){
        const auto& pid = (*it).first;
        if(!current_pids_.count(pid)){
            it = processes_.erase(it);
            //Remove the pid from the tracked list
            ignore_process(pid);
        }else{
            it++;
        }
    }

    sigar_proc_list_destroy(sigar_, &process_list);
    
    //Unset our flag
    force_process_name_check_ = false;
    

    return true;
}

void SigarSystemInfo::monitor_process(const int pid){
    tracked_pids_.insert(pid);
}

void SigarSystemInfo::ignore_process(const int pid){
    tracked_pids_.erase(pid);
}

std::set<int> SigarSystemInfo::get_monitored_pids() const{
    return tracked_pids_;
}


void SigarSystemInfo::monitor_processes(const std::string& process_name){
    if(!tracked_process_names_.count(process_name)){
        tracked_process_names_.insert(process_name);
        force_process_name_check_ = true;
    }
}

void SigarSystemInfo::ignore_processes(const std::string& process_name){
    if(tracked_process_names_.count(process_name)){
        tracked_process_names_.erase(process_name);
        force_process_name_check_ = true;
    }
}

void SigarSystemInfo::clear_monitored_processes(){
    tracked_process_names_.clear();
}

const std::set<std::string>& SigarSystemInfo::get_monitored_processes_names() const{
    return tracked_process_names_;
}


 

bool SigarSystemInfo::stringInString(const std::string& haystack, const std::string& needle) const{
    auto match = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](char ch1, char ch2) {
        #if _WIN32
		return toupper(ch1) == toupper(ch2);
        #else
		return std::toupper(ch1) == std::toupper(ch2);
        #endif
        }
    );
    return (match != haystack.end() );
}
