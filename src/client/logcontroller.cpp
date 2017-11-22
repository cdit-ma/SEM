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
 
#include "logcontroller.h"

#include <chrono>
#include <iostream>

#include "sigarsysteminfo.h"
#include "systeminfo.h"

#include <re_common/proto/systemstatus/systemstatus.pb.h>
#include <re_common/zmq/protowriter/cachedprotowriter.h>
#include <re_common/zmq/monitor/monitor.h>
#include <zmq.hpp>

#include <google/protobuf/util/json_util.h>

//Constructor used for print only call
LogController::LogController(){
    //Construct our SystemInfo class
    system_info_ = new SigarSystemInfo();
    host_name = system_info_->get_hostname();
}

std::string LogController::GetSystemInfoJson(){
    //Let sigar do its thing for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    system_info_->update();

    auto info = GetOneTimeInfo();

    std::string output;
    google::protobuf::util::JsonOptions options;
    options.add_whitespace = true;

    google::protobuf::util::MessageToJsonString(*info, &output, options);
    return output;
}

bool LogController::Start(std::string endpoint, double frequency, std::vector<std::string> processes, bool live_mode){
    if(!running){
        if(live_mode){
            writer_ = new zmq::ProtoWriter();
        }else{
            writer_ = new zmq::CachedProtoWriter();
        }

        //Monitor
        monitor_ = new zmq::Monitor();
        monitor_->RegisterEventCallback(std::bind(&LogController::GotNewConnection, this, std::placeholders::_1, std::placeholders::_2));
        writer_->AttachMonitor(monitor_, ZMQ_EVENT_ACCEPTED);

        if(!writer_->BindPublisherSocket(endpoint)){
            return false;
        }

        //Zero check before division
        if(frequency <= 0){
            frequency = 1;
        }
    
        //Convert frequency to period
        sleep_time_ = (int)((1 / frequency) * 1000);
        processes_ = processes;
    
        writer_thread_ = new std::thread(&LogController::WriteThread, this);
        logging_thread_ = new std::thread(&LogController::LogThread, this);
        running = true;
        return true;
    }
    return false;
}

bool LogController::Terminate(){
    if(running){
        TerminateLogger();
        TerminateWriter();
        delete logging_thread_;
        delete writer_thread_;
        delete system_info_;
        if(writer_){
            writer_->Terminate();
            std::cout << "* Logged " << writer_->GetTxCount() << " messages." << std::endl;
            delete writer_;
        }
        running = false;
        return true;
    }
    return false;
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
    Terminate();
   
}

void LogController::GotNewConnection(int, std::string){
    QueueOneTimeInfo();
}

void LogController::LogThread(){
    //Subscribe to our desired process names
    for(std::string process_name : processes_){
        system_info_->monitor_processes(process_name);
    }

    namespace s_c = std::chrono;

    //Don't get our first log reading for 1 second
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    re_common::SystemInfo* one_time_info = 0;

    //Update loop.
    while(!logger_terminate_){
        auto before_time = 
            s_c::duration_cast<s_c::milliseconds>(s_c::system_clock::now().time_since_epoch());
        if(system_info_->update()){
	
            //Get a new filled protobuf message
            re_common::SystemStatus* status = GetSystemStatus();
            if(!one_time_info){
                one_time_info = GetOneTimeInfo();
            }

            //Lock the Queue, and notify the writer queue.
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            if(one_time_flag_){
                message_queue_.push(new re_common::SystemInfo(*one_time_info));
                one_time_flag_ = false;
            }
            message_queue_.push(status);

            queue_lock_condition_.notify_all();
        }
        auto after_time = 
            s_c::duration_cast<s_c::milliseconds>(s_c::system_clock::now().time_since_epoch());
        auto duration = after_time - before_time;

        std::this_thread::sleep_for(s_c::milliseconds(sleep_time_)-s_c::milliseconds(duration));
    }
}

void LogController::WriteThread(){
    while(!writer_terminate_){
        std::queue<google::protobuf::MessageLite* > replace_queue;
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
            if(m){
                writer_->PushMessage(host_name, m);
            }
            
            replace_queue.pop();
        }
    }
}

re_common::SystemInfo* LogController::GetOneTimeInfo(){
    
    re_common::SystemInfo* onetime_system_info = new re_common::SystemInfo();

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
        re_common::FileSystemInfo* fss = onetime_system_info->add_file_system_info();    
        //send onetime info
        fss->set_name(info->get_fs_name(i));
        fss->set_type((re_common::FileSystemInfo::Type)info->get_fs_type(i));
        fss->set_size(info->get_fs_size(i));
    }

    int interface_count = info->get_interface_count();
    for(int i = 0; i < interface_count; i++){ 
        if(info->get_interface_state(i, SystemInfo::InterfaceState::UP)){
            re_common::InterfaceInfo* is = onetime_system_info->add_interface_info();
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

void LogController::QueueOneTimeInfo(){
    //Get Lock
    std::unique_lock<std::mutex> lock(queue_mutex_);
    one_time_flag_ = true;
}

re_common::SystemStatus* LogController::GetSystemStatus(){
    //Construct a protobuf message to fill with information
    re_common:: SystemStatus* status = new re_common::SystemStatus();

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
        ++it;
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
            re_common::ProcessStatus* ps = status->add_processes();
            ps->set_pid(pid);

            ps->set_state((re_common::ProcessStatus::State)info->get_process_state(pid));

            re_common::ProcessInfo* pi = status->add_process_info();
            if(!seen_pid){
                //send onetime info
                pi->set_pid(pid);
                pi->set_name(info->get_process_name(pid));
                pi->set_args(info->get_process_arguments(pid));
                pi->set_start_time(info->get_monitored_process_start_time(pid));
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
        re_common::FileSystemStatus* fss = status->add_file_systems();
        fss->set_name(info->get_fs_name(i));
        fss->set_utilization(info->get_fs_utilization(i));
    }

    int interface_count = info->get_interface_count();
    for(int i = 0; i < interface_count; i++){ 
        if(info->get_interface_state(i, SystemInfo::InterfaceState::UP)){
            re_common::InterfaceStatus* is = status->add_interfaces();
            is->set_name(info->get_interface_name(i));
            is->set_rx_bytes(info->get_interface_rx_bytes(i));
            is->set_rx_packets(info->get_interface_rx_packets(i));
            is->set_tx_bytes(info->get_interface_tx_bytes(i));
            is->set_tx_packets(info->get_interface_tx_packets(i));
        }
    }

    return status;
}
