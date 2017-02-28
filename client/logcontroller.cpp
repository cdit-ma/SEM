#include "logcontroller.h"

#include <chrono>
#include <iostream>

#include "sigarsysteminfo.h"
#include "systeminfo.h"
#include "../re_common/proto/systemstatus/systemstatus.pb.h"
#include "../re_common/zmq/protowriter/cachedzmqmessagewriter.h"

LogController::LogController(int port, double frequency, std::vector<std::string> processes, bool cached){
    if(cached){
        writer_ = new CachedZMQMessageWriter();
    }else{
        writer_ = new ZMQMessageWriter();
    }

    std::string port_string = "tcp://*:" + std::to_string(port);

    writer_->BindPublisherSocket(port_string);

    //Construct our SystemInfo class
    system_info_ = new SigarSystemInfo();
	
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    writer_thread_ = new std::thread(&LogController::WriteThread, this);
    logging_thread_ = new std::thread(&LogController::LogThread, this);

    //Zero check before division
    if(frequency <= 0){
        frequency = 1;
    }
    
    //Convert frequency to period
    sleep_time_ = (int)((1 / frequency) * 1000);
    processes_ = processes;
}

void LogController::GotNewServer(std::string endpoint){
    std::cout << "GotNewServer, Sending once off data to: " << endpoint << std::endl;
    //Queue magic data brew
    QueueOneTimeInfo(endpoint);
}

void LogController::Terminate(){
    TerminateLogger();
    TerminateWriter();
}

void LogController::TerminateLogger(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        logger_terminate_ = true;
        queue_lock_condition_.notify_all();
    }
    logging_thread_->join();
}

void LogController::TerminateWriter(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        writer_terminate_ = true;
        queue_lock_condition_.notify_all();
    }
    writer_thread_->join();
}

LogController::~LogController(){
    delete logging_thread_;
    delete writer_thread_;
    delete system_info_;
    delete writer_;
}

void LogController::LogThread(){

    //Subscribe to our desired process names
    for(std::string process_name : processes_){
        system_info_->monitor_processes(process_name);
    }

    namespace s_c = std::chrono;

    //Update loop.
    while(!logger_terminate_){
        auto before_time = 
            s_c::duration_cast<s_c::milliseconds>(s_c::system_clock::now().time_since_epoch());
        if(system_info_->update()){
	
            //Get a new filled protobuf message
            SystemStatus* status = GetSystemStatus();

            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            
            //Lock the Queue, and notify the writer queue.
            message_queue_.push(std::make_pair("*", status));
            queue_lock_condition_.notify_all();
        }
        auto after_time = 
            s_c::duration_cast<s_c::milliseconds>(s_c::system_clock::now().time_since_epoch());
        auto duration = after_time - before_time;

        std::this_thread::sleep_for(s_c::milliseconds(sleep_time_)-s_c::milliseconds(duration));
    }
    std::cout << "Logging thread terminated." << std::endl;
}

void LogController::WriteThread(){
    while(!writer_terminate_){
        std::queue<std::pair<std::string, google::protobuf::MessageLite*> > replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for notify
            queue_lock_condition_.wait(lock);
            //Swap our queues
            if(!message_queue_.empty()){
                message_queue_.swap(replace_queue);
            }
        }

        //Empty our write queue
        while(!replace_queue.empty()){
            auto m = replace_queue.front();
            if(m.second){
                writer_->PushMessage(&(m.first), m.second);
            }
            
            replace_queue.pop();
        }
    }
    std::cout << "Writer thread terminated." << std::endl;
}

OneTimeSystemInfo* LogController::GetOneTimeInfo(){
    OneTimeSystemInfo* onetime_system_info = new OneTimeSystemInfo();

    auto info = system_info_;

    onetime_system_info->set_hostname(info->get_hostname());
    onetime_system_info->set_timestamp(info->get_update_timestamp());

    //Send OS Info
    onetime_system_info->set_os_name(info->get_os_name());
    onetime_system_info->set_os_arch(info->get_os_arch());
    onetime_system_info->set_os_description(info->get_os_description());
    onetime_system_info->set_os_version(info->get_os_version());
    onetime_system_info->set_os_vendor(info->get_os_vendor());
    onetime_system_info->set_os_vendor_name(info->get_os_vendor_name());

    //Send Hardware Info
    onetime_system_info->set_cpu_model(info->get_cpu_model());
    onetime_system_info->set_cpu_vendor(info->get_cpu_vendor());
    onetime_system_info->set_cpu_frequency(info->get_cpu_frequency());
    onetime_system_info->set_physical_memory(info->get_phys_mem());

    int fs_count = info->get_fs_count();
    for(int i = 0; i < fs_count; i++){
        FileSystemInfo* fss = onetime_system_info->add_file_system_info();    
        //send onetime info
        fss->set_name(info->get_fs_name(i));
        fss->set_type((FileSystemInfo::Type)info->get_fs_type(i));
        fss->set_size(info->get_fs_size(i));
    }

    int interface_count = info->get_interface_count();
    for(int i = 0; i < interface_count; i++){ 
        if(info->get_interface_state(i, SystemInfo::InterfaceState::UP)){
            InterfaceInfo* is = onetime_system_info->add_interface_info();
            //get onetime info
            is->set_name(info->get_interface_name(i));            
            is->set_type(info->get_interface_type(i));
            is->set_description(info->get_interface_description(i));
            is->set_ipv4_addr(info->get_interface_ipv4(i));
            is->set_ipv6_addr(info->get_interface_ipv6(i));
            is->set_mac_addr(info->get_interface_mac(i));
            is->set_speed(info->get_interface_speed(i));
        }
    }

/*
    ps->set_name(info->get_process_name(pid));
    ps->set_args(info->get_process_arguments(pid));
    ps->set_start_time(info->get_monitored_process_start_time(pid));
*/
    return onetime_system_info;

}

void LogController::QueueOneTimeInfo(std::string topic){
    if(!one_time_info_){
        std::cout << "Populating one time info" << std::endl;                
        one_time_info_ = GetOneTimeInfo();
        std::cout << "Populated one time info" << std::endl;
    }
        message_queue_.push(std::make_pair(topic, new OneTimeSystemInfo(*one_time_info_)));
}

SystemStatus* LogController::GetSystemStatus(){
    //Construct a protobuf message to fill with information
    SystemStatus* status = new SystemStatus();

    auto info = system_info_;

    status->set_hostname(info->get_hostname());
    status->set_timestamp(info->get_update_timestamp());
    
    //Increment the message_id
    status->set_message_id(++message_id_);
    

    for(int i = 0; i < info->get_cpu_count(); i++){
        status->add_cpu_core_utilization(info->get_cpu_utilization(i));
    }
    status->set_cpu_utilization(info->get_cpu_overall_utilization());
    status->set_phys_mem_utilization(info->get_phys_mem_utilization());
    
    
    std::vector<int> pids = info->get_monitored_pids();
    //prune update time map based on currently alive processes
    for(auto it = pid_updated_times_.begin(); it != pid_updated_times_.end();){
        bool delete_flag = true;

        for(auto pid : pids){
            if(pid == it->first){
                delete_flag = false;
            }
        }
        if(delete_flag){
            pid_updated_times_.erase(it);
        }
        else{
            ++it;
        }
    }

    for(size_t i = 0; i < pids.size(); i++){
        int pid = pids[i];

        double last_updated_time = info->get_monitored_process_update_time(pid);
        

        bool seen_pid = pid_updated_times_.count(pid) > 0;
        bool send_pid_update = !seen_pid;
        
        if(seen_pid){
            double last_sent_time = pid_updated_times_.at(pid);
            
            if(last_updated_time > last_sent_time){
                send_pid_update = true;
            }
        }
        
        if(send_pid_update){
            ProcessStatus* ps = status->add_processes();
            ps->set_pid(pid);

            ps->set_state((ProcessStatus::State)info->get_process_state(pid));

            /*if(!seen_pid){
                //send onetime info
                ps->mutable_info()->set_name(info->get_process_name(pid));
                ps->mutable_info()->set_args(info->get_process_arguments(pid));
                ps->mutable_info()->set_start_time(info->get_monitored_process_start_time(pid));
            }*/
            ps->set_cpu_core_id(info->get_monitored_process_cpu(pid));
            ps->set_cpu_utilization(info->get_monitored_process_cpu_utilization(pid));
            ps->set_phys_mem_utilization(info->get_monitored_process_phys_mem_utilization(pid));
            ps->set_thread_count(info->get_monitored_process_thread_count(pid));
            ps->set_disk_read(info->get_monitored_process_disk_read(pid));
            ps->set_disk_written(info->get_monitored_process_disk_written(pid));
            ps->set_disk_total(info->get_monitored_process_disk_total(pid));
            
            //Update our Map to include this PID
            pid_updated_times_[pid] = last_updated_time;
        }
    }

    int fs_count = info->get_fs_count();
    for(int i = 0; i < fs_count; i++){
        FileSystemStatus* fss = status->add_file_systems();
        fss->set_name(info->get_fs_name(i));
        fss->set_utilization(info->get_fs_utilization(i));
    }

    int interface_count = info->get_interface_count();
    for(int i = 0; i < interface_count; i++){ 
        if(info->get_interface_state(i, SystemInfo::InterfaceState::UP)){
            InterfaceStatus* is = status->add_interfaces();
            is->set_name(info->get_interface_name(i));
            is->set_rx_bytes(info->get_interface_rx_bytes(i));
            is->set_rx_packets(info->get_interface_rx_packets(i));
            is->set_tx_bytes(info->get_interface_tx_bytes(i));
            is->set_tx_packets(info->get_interface_tx_packets(i));
        }
    }

    return status;
}
