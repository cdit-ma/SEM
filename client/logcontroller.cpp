#include "logcontroller.h"
#include <iostream>
#include <chrono>
#include <thread>


#include <list>
#include "sigarsysteminfo.h"
#include "cachedzmqmessagewriter.h"

LogController::LogController(double frequency, std::vector<std::string> processes, bool cached){
    writer_ = new ZMQMessageWriter();
    writer_->bind_publisher_socket("tcp://*:5555");

    logging_thread_ = new std::thread(&LogController::LogThread, this);
    writer_thread_ = new std::thread(&LogController::WriteThread, this);
    message_id_ = 0;

    //Zero check before division
    if(frequency <= 0){
        frequency = 1;
    }
    //Convert frequency to period
    sleep_time_ = (1 / frequency) * 1000;
    processes_ = processes;
    terminate_ = false;
}

LogController::~LogController(){
    terminate_ = true;
    logging_thread_->join();
    delete logging_thread_;
    writer_thread_->join();
    delete writer_thread_;    
}

void LogController::LogThread(){
    //Construct our SystemInfo class
    SystemInfo* system_info = new SigarSystemInfo();


    //Subscribe to our desired process names
    for(std::string process_name : processes_){
        system_info->monitor_processes(process_name);
    }

    //Update loop.
    while(!terminate_){
        //Sleep for period
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_));
        if(system_info->update()){
            //Get a new filled protobuf message
            SystemStatus* status = GetSystemStatus(system_info);
            
            //Lock the Queue, and notify the writer queue.
            std::unique_lock<std::mutex> lock(queue_mutex_);
            message_queue_.push(status);
            queue_lock_condition_.notify_all();
        }
    }
    std::cout << "Logging thread terminated." << std::endl;
}

void LogController::WriteThread(){
    while(!terminate_){
        std::queue<SystemStatus*> replace_queue;
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
            writer_->push_message(replace_queue.front());
            replace_queue.pop();
        }
    }
    std::cout << "Writer thread terminated." << std::endl;
}

SystemStatus* LogController::GetSystemStatus(SystemInfo* info){
    //Construct a protobuf message to fill with information
    SystemStatus* status = new SystemStatus();

    status->set_hostname(info->get_hostname());
    status->set_timestamp(info->get_update_timestamp());
    
    //Increment the message_id
    status->set_message_id(++message_id_);

    //Send the SystemInfo once
    if(!seen_hostnames_.count(info->get_hostname())){
        SystemStatus::SystemInfo* sys_info = status->mutable_info();

        //Send OS Info
        sys_info->set_os_name(info->get_os_name());
        sys_info->set_os_arch(info->get_os_arch());
        sys_info->set_os_description(info->get_os_description());
        sys_info->set_os_version(info->get_os_version());
        sys_info->set_os_vendor(info->get_os_vendor());
        sys_info->set_os_vendor_name(info->get_os_vendor_name());

        //Send Hardware Info
        sys_info->set_cpu_model(info->get_cpu_model());
        sys_info->set_cpu_vendor(info->get_cpu_vendor());
        sys_info->set_cpu_frequency(info->get_cpu_frequency());
        sys_info->set_physical_memory(info->get_phys_mem());

        //Store the hostname
        seen_hostnames_.insert(info->get_hostname());
    }
    

    for(int i = 0; i < info->get_cpu_count(); i++){
        status->add_cpu_core_utilization(info->get_cpu_utilization(i));
    }
    status->set_cpu_utilization(info->get_cpu_overall_utilization());
    status->set_phys_mem_utilization(info->get_phys_mem_utilization());
    
    
    std::vector<int> pids = info->get_monitored_pids();
    for(int i = 0; i < pids.size(); i++){
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

            if(!seen_pid){
                //send onetime info
                ps->mutable_info()->set_name(info->get_process_name(pid));
                ps->mutable_info()->set_args(info->get_process_arguments(pid));
                ps->mutable_info()->set_start_time(info->get_monitored_process_start_time(pid));
            }
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
        
        if(!seen_fs_.count(info->get_fs_name(i))){
            //send onetime info
            fss->mutable_info()->set_type((FileSystemStatus::FileSystemInfo::Type)info->get_fs_type(i));
            fss->mutable_info()->set_size(info->get_fs_size(i));
            seen_fs_.insert(info->get_fs_name(i));
        }
        
        fss->set_name(info->get_fs_name(i));
        fss->set_utilization(info->get_fs_utilization(i));
    }

    int interface_count = info->get_interface_count();
    for(int i = 0; i < interface_count; i++){
        InterfaceStatus* is = status->add_interfaces();
        
        if(!seen_if_.count(info->get_interface_name(i))){
            //get onetime info
            is->mutable_info()->set_type(info->get_interface_type(i));
            is->mutable_info()->set_description(info->get_interface_description(i));
            is->mutable_info()->set_ipv4_addr(info->get_interface_ipv4(i));
            is->mutable_info()->set_ipv6_addr(info->get_interface_ipv6(i));
            is->mutable_info()->set_mac_addr(info->get_interface_mac(i));
            is->mutable_info()->set_speed(info->get_interface_speed(i));
            seen_if_.insert(info->get_interface_name(i));
        }

        is->set_name(info->get_interface_name(i));
        is->set_rx_bytes(info->get_interface_rx_bytes(i));
        is->set_rx_packets(info->get_interface_rx_packets(i));
        is->set_tx_bytes(info->get_interface_tx_bytes(i));
        is->set_tx_packets(info->get_interface_tx_packets(i));
    }

    return status;
}