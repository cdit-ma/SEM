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
}

void LogController::LogThread(){
    int i = 0;

    SystemInfo* system_info = new SigarSystemInfo();

    for(std::string process_name : processes_){
        system_info->monitor_processes(process_name);
    }

    system_info->monitor_processes("LoggerClient");

    //Update loop.
    while(true){
        //Sleep for period
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_));
        if(system_info->update()){
            SystemStatus* status = GetSystemStatus(system_info);
            //Lock the Queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            message_queue_.push(status);
            queue_lock_condition_.notify_all();
        }else{
            //Don't update more than 10 times a second.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void LogController::WriteThread(){
    int count = 0;
    while(true){
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
            count ++;
            writer_->push_message(replace_queue.front());
            replace_queue.pop();
        }
    }


}

SystemStatus* LogController::GetSystemStatus(SystemInfo* info){
    message_id_++;
    SystemStatus* status = new SystemStatus();
    status->set_hostname(info->get_hostname());
    status->set_timestamp(info->get_update_timestamp());
    status->set_message_id(message_id_);

    if(!seen_hostnames_.count(info->get_hostname())){
        //send onetime info
        status->mutable_info()->set_os_name(info->get_os_name());
        status->mutable_info()->set_os_arch(info->get_os_arch());
        status->mutable_info()->set_os_description(info->get_os_description());
        status->mutable_info()->set_os_version(info->get_os_version());
        status->mutable_info()->set_os_vendor(info->get_os_vendor());
        status->mutable_info()->set_os_vendor_name(info->get_os_vendor_name());

        status->mutable_info()->set_cpu_model(info->get_cpu_model());
        status->mutable_info()->set_cpu_vendor(info->get_cpu_vendor());
        status->mutable_info()->set_cpu_frequency(info->get_cpu_frequency());

        status->mutable_info()->set_physical_memory(info->get_phys_mem());
        seen_hostnames_.insert(info->get_hostname());
    }
    

    for(int i = 0; i < info->get_cpu_count(); i++){
        status->add_cpu_core_utilization(info->get_cpu_utilization(i));
    }
    status->set_cpu_utilization(info->get_cpu_overall_utilization());
    status->set_phys_mem_utilization(info->get_phys_mem_utilization());
    
    std::vector<int> pids = info->get_monitored_pids();
    for(int i = 0; i < pids.size(); i++){
        
        ProcessStatus* ps = status->add_processes();
        ps->set_pid(pids[i]);

        ps->set_state((ProcessStatus::State)info->get_process_state(pids[i]));

        if(!seen_pids_.count(pids[i])){
            //send onetime info
            ps->mutable_info()->set_name(info->get_process_name(pids[i]));
            ps->mutable_info()->set_args(info->get_process_arguments(pids[i]));
            ps->mutable_info()->set_start_time(info->get_monitored_process_start_time(pids[i]));
            seen_pids_.insert(pids[i]);
        }

        ps->set_cpu_core_id(info->get_monitored_process_cpu(pids[i]));
        ps->set_cpu_utilization(info->get_monitored_process_cpu_utilization(pids[i]));
        ps->set_phys_mem_utilization(info->get_monitored_process_phys_mem_utilization(pids[i]));
        ps->set_thread_count(info->get_monitored_process_thread_count(pids[i]));
        ps->set_disk_read(info->get_monitored_process_disk_read(pids[i]));
        ps->set_disk_written(info->get_monitored_process_disk_written(pids[i]));
        ps->set_disk_total(info->get_monitored_process_disk_total(pids[i]));
                
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
