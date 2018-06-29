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
 
#ifdef _WIN32
#define NOMINMAX
#endif

#include "logcontroller.h"

#include <chrono>
#include <iostream>
#include <algorithm>

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
    system_ = new SigarSystemInfo();
}

std::string LogController::GetSystemInfoJson(){
    //Let sigar do its thing for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    system_->update();
    auto info = GetSystemInfo();

    std::string output;
    google::protobuf::util::JsonOptions options;
    options.add_whitespace = true;

    google::protobuf::util::MessageToJsonString(*info, &output, options);
    delete info;
    return output;
}

bool LogController::Start(const std::string& publisher_endpoint, const double& frequency, const std::vector<std::string>& processes, const bool& live_mode){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(!logging_future_.valid()){
        //Reset
        message_id_ = 0;
        pid_updated_times_.clear();
        std::lock_guard<std::mutex> lock(interupt_mutex_);
        interupt_ = false;
        logging_future_ = std::async(std::launch::async, &LogController::LogThread, this, publisher_endpoint, frequency, processes, live_mode);
        return true;
    }
    return false;
}

bool LogController::Stop(){
    std::lock_guard<std::mutex> lock(state_mutex_);
    if(logging_future_.valid()){
        InteruptLogThread();
        logging_future_.get();
        return true;
    }
    return false;
}

void LogController::InteruptLogThread(){
    std::lock_guard<std::mutex> lock(interupt_mutex_);
    interupt_ = true;
    log_condition_.notify_all();
}

LogController::~LogController(){
    Stop();
    delete system_;
}

void LogController::GotNewConnection(int, std::string){
    //Enqueue
    QueueOneTimeInfo();
}

void LogController::QueueOneTimeInfo(){
    std::lock_guard<std::mutex> lock(one_time_mutex_);
    send_onetime_info_ = true;
}

void LogController::LogThread(const std::string& publisher_endpoint, const double& frequency, const std::vector<std::string>& processes, const bool& live_mode){
    auto writer = live_mode ? std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter()) : std::unique_ptr<zmq::ProtoWriter>(new zmq::CachedProtoWriter());
    {
        zmq::Monitor monitor;
        monitor.RegisterEventCallback(std::bind(&LogController::GotNewConnection, this, std::placeholders::_1, std::placeholders::_2));
        
        writer->AttachMonitor(&monitor, ZMQ_EVENT_ACCEPTED);

        if(!writer->BindPublisherSocket(publisher_endpoint)){
            std::cerr << "Writer cannot bind publisher endpoint '" << publisher_endpoint << "'" << std::endl;
            return;
        }
        
        //Get the duration in milliseconds
        auto tick_duration = std::chrono::milliseconds(static_cast<int>(1000.0 / std::max(0.0, frequency)));

        system_->ignore_processes();
        
        //Subscribe to our desired process names
        for(const auto& process_name : processes){
            system_->monitor_processes(process_name);
        }
        

        //We need to sleep for at least 1 second, if our duration is less than 1 second, calculate the offset to get at least 1 second
        auto last_duration = std::min(std::chrono::milliseconds(-1000) + tick_duration, std::chrono::milliseconds(0));
        while(true){
            auto sleep_duration = tick_duration - last_duration;
            {
                std::unique_lock<std::mutex> lock(interupt_mutex_);
                log_condition_.wait_for(lock, sleep_duration, [this]{return interupt_;});
                if(interupt_){
                    break;
                }
            }

            auto start = std::chrono::steady_clock::now();
            {
                //Whenever a new server connects, send one time information, using our client address as the topic
                std::lock_guard<std::mutex> lock(one_time_mutex_);
                if(send_onetime_info_){
                    writer->PushMessage(publisher_endpoint, GetSystemInfo());
                    send_onetime_info_ = false;
                }
            }

            {
                //Send the tick'd information to all servers
                if(system_->update()){
                    writer->PushMessage(GetSystemStatus());
                }
            }

            //Calculate the duration we should sleep for
            auto end = std::chrono::steady_clock::now();
            last_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        }
        writer->Terminate();
    }
    std::cout << "* Logged " << writer->GetTxCount() << " messages." << std::endl;
}


re_common::SystemStatus* LogController::GetSystemStatus(){
    auto system_status = new re_common::SystemStatus();
    
    system_status->set_hostname(system_->get_hostname());
    system_status->set_timestamp(system_->get_update_timestamp());
    
    //Increment the message_id
    system_status->set_message_id(++message_id_);
    
    for(auto i = 0; i < system_->get_cpu_count(); i++){
        system_status->add_cpu_core_utilization(system_->get_cpu_utilization(i));
    }

    system_status->set_cpu_utilization(system_->get_cpu_overall_utilization());
    system_status->set_phys_mem_utilization(system_->get_phys_mem_utilization());
    
    
    auto alive_pids = system_->get_monitored_pids();

    //Prune our update time map
    for(auto it = pid_updated_times_.begin(); it != pid_updated_times_.end();){
        if(!alive_pids.count(it->first)){
            pid_updated_times_.erase(it++);
        }else{
            ++it;
        }
    }

    for(const auto& pid : alive_pids){
        auto last_update_time = system_->get_monitored_process_update_time(pid);

        auto seen_pid_before = pid_updated_times_.count(pid) > 0;
        auto send_pid = true;

        //We shouldn't send the pid info if we haven't updated it since our last send
        if(seen_pid_before){
            auto last_sent_time = pid_updated_times_[pid];
            if(last_update_time <= last_sent_time){
                send_pid = false;
            }
        }

        if(send_pid){
            auto process_status = system_status->add_processes();
            process_status->set_pid(pid);
            process_status->set_state((re_common::ProcessStatus::State)system_->get_process_state(pid));

            //send onetime info
            if(!seen_pid_before){
                auto process_info = system_status->add_process_info();
                process_info->set_pid(pid);
                process_info->set_name(system_->get_process_name(pid));
                process_info->set_args(system_->get_process_arguments(pid));
                process_info->set_start_time(system_->get_monitored_process_start_time(pid));
            }
            //Set the status info
            process_status->set_cpu_core_id(system_->get_monitored_process_cpu(pid));
            process_status->set_cpu_utilization(system_->get_monitored_process_cpu_utilization(pid));
            process_status->set_phys_mem_utilization(system_->get_monitored_process_phys_mem_utilization(pid));
            process_status->set_thread_count(system_->get_monitored_process_thread_count(pid));
            process_status->set_disk_read(system_->get_monitored_process_disk_read(pid));
            process_status->set_disk_written(system_->get_monitored_process_disk_written(pid));
            process_status->set_disk_total(system_->get_monitored_process_disk_total(pid));
            
            //Update our Map to include this PID
            pid_updated_times_[pid] = last_update_time;
        }
    }

    int fs_count = system_->get_fs_count();
    for(auto i = 0; i < fs_count; i++){
        auto file_system_status = system_status->add_file_systems();
        file_system_status->set_name(system_->get_fs_name(i));
        file_system_status->set_utilization(system_->get_fs_utilization(i));
    }

    int interface_count = system_->get_interface_count();
    for(auto i = 0; i < interface_count; i++){ 
        if(system_->get_interface_state(i, SystemInfo::InterfaceState::UP)){
            auto interface_status = system_status->add_interfaces();
            interface_status->set_name(system_->get_interface_name(i));
            interface_status->set_rx_bytes(system_->get_interface_rx_bytes(i));
            interface_status->set_rx_packets(system_->get_interface_rx_packets(i));
            interface_status->set_tx_bytes(system_->get_interface_tx_bytes(i));
            interface_status->set_tx_packets(system_->get_interface_tx_packets(i));
        }
    }
    return system_status;
}

re_common::SystemInfo* LogController::GetSystemInfo(){
    auto system_info = new re_common::SystemInfo();
    
    system_info->set_hostname(system_->get_hostname());
    system_info->set_timestamp(system_->get_update_timestamp());

    //OS info
    system_info->set_os_name(system_->get_os_name());
    system_info->set_os_arch(system_->get_os_arch());
    system_info->set_os_description(system_->get_os_description());
    system_info->set_os_version(system_->get_os_version());
    system_info->set_os_vendor(system_->get_os_vendor());
    system_info->set_os_vendor_name(system_->get_os_vendor_name());

    //Hardware info
    system_info->set_cpu_model(system_->get_cpu_model());
    system_info->set_cpu_vendor(system_->get_cpu_vendor());
    system_info->set_cpu_frequency(system_->get_cpu_frequency());
    system_info->set_physical_memory(system_->get_phys_mem());

    //File systems info
    int fs_count = system_->get_fs_count();
    for(int i = 0; i < fs_count; i++){
        auto file_system_info = system_info->add_file_system_info();    
        //send onetime info
        file_system_info->set_name(system_->get_fs_name(i));
        file_system_info->set_type((re_common::FileSystemInfo::Type)system_->get_fs_type(i));
        file_system_info->set_size(system_->get_fs_size(i));
    }

    //Network interface info
    int interface_count = system_->get_interface_count();
    for(int i = 0; i < interface_count; i++){ 
        if(system_->get_interface_state(i, SystemInfo::InterfaceState::UP)){
            auto interface_info = system_info->add_interface_info();
            //get onetime info
            interface_info->set_name(system_->get_interface_name(i));            
            interface_info->set_type(system_->get_interface_type(i));
            interface_info->set_description(system_->get_interface_description(i));
            interface_info->set_ipv4_addr(system_->get_interface_ipv4(i));
            interface_info->set_ipv6_addr(system_->get_interface_ipv6(i));
            interface_info->set_mac_addr(system_->get_interface_mac(i));
            interface_info->set_speed(system_->get_interface_speed(i));
        }
    }
    return system_info;
}