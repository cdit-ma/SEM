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

#include "systeminfo.h"
#include <proto/systemstatus/systemstatus.pb.h>


double SystemInfo::convert_timestamp(const std::chrono::milliseconds& timestamp){
    return timestamp.count() / 1000.0;
}

//Refresh
re_common::SystemStatus* SystemInfo::GetSystemStatus(const int listener_id){
    std::lock_guard<std::mutex> lock(mutex_);

    const auto& current_data_time = get_update_timestamp();
    std::chrono::milliseconds last_update_sent(0);

    if(listener_updated_times_.count(listener_id)){
        last_update_sent = listener_updated_times_.at(listener_id);
    }

    //If the last updated data time is the same, ignore
    if(last_update_sent == current_data_time){
        return nullptr;
    }
    
    
    auto system_status = new re_common::SystemStatus();
    
    system_status->set_hostname(get_hostname());
    system_status->set_timestamp(convert_timestamp(current_data_time));
    

    if(!listener_message_count_.count(listener_id)){
        listener_message_count_[listener_id] = 0;
    }
    //Increment the message_id
    system_status->set_message_id(++(listener_message_count_[listener_id]));
    
    const auto& cpu_count = get_cpu_count();
    for(auto i = 0; i < cpu_count; i++){
        system_status->add_cpu_core_utilization(get_cpu_utilization(i));
    }

    system_status->set_cpu_utilization(get_cpu_overall_utilization());
    system_status->set_phys_mem_utilization(get_phys_mem_utilization());
    
    
    const auto& alive_pids = get_monitored_pids();

    for(const auto& pid : alive_pids){
        bool update_pid = get_monitored_process_update_time(pid) > last_update_sent;

        if(update_pid){
            auto process_status = system_status->add_processes();
            process_status->set_pid(pid);
            process_status->set_name(get_process_name(pid));
            process_status->set_state((re_common::ProcessStatus::State)get_process_state(pid));

            //Only send the one time info if we haven't seen this message
            bool send_process_info = !(pid_lookups_.count(listener_id) && pid_lookups_.at(listener_id).count(pid));

            if(send_process_info){
                auto process_info = system_status->add_process_info();
                process_info->set_pid(pid);
                process_info->set_name(get_process_name(pid));
                process_info->set_args(get_process_arguments(pid));
                process_info->set_start_time(get_monitored_process_start_time(pid));
            }

            //Set the status info
            process_status->set_cpu_core_id(get_monitored_process_cpu(pid));
            process_status->set_cpu_utilization(get_monitored_process_cpu_utilization(pid));
            process_status->set_phys_mem_utilization(get_monitored_process_phys_mem_utilization(pid));
            process_status->set_thread_count(get_monitored_process_thread_count(pid));
            process_status->set_disk_read(get_monitored_process_disk_read(pid));
            process_status->set_disk_written(get_monitored_process_disk_written(pid));
            process_status->set_disk_total(get_monitored_process_disk_total(pid));
        }
    }

    //Copy across the list of alive pids
    pid_lookups_[listener_id] = alive_pids;

    int fs_count = get_fs_count();
    for(auto i = 0; i < fs_count; i++){
        auto file_system_status = system_status->add_file_systems();
        file_system_status->set_name(get_fs_name(i));
        file_system_status->set_utilization(get_fs_utilization(i));
    }

    int interface_count = get_interface_count();
    for(auto i = 0; i < interface_count; i++){ 
        if(get_interface_state(i, SystemInfo::InterfaceState::UP)){
            auto interface_status = system_status->add_interfaces();
            interface_status->set_name(get_interface_name(i));
            interface_status->set_rx_bytes(get_interface_rx_bytes(i));
            interface_status->set_rx_packets(get_interface_rx_packets(i));
            interface_status->set_tx_bytes(get_interface_tx_bytes(i));
            interface_status->set_tx_packets(get_interface_tx_packets(i));
        }
    }
    //Updates
    listener_updated_times_[listener_id] = current_data_time;
    return system_status;
}

re_common::SystemInfo* SystemInfo::GetSystemInfo(const int listener_id){
    std::lock_guard<std::mutex> lock(mutex_);

    auto system_info = new re_common::SystemInfo();
    
    system_info->set_hostname(get_hostname());
    system_info->set_timestamp(convert_timestamp(get_update_timestamp()));

    //OS info
    system_info->set_os_name(get_os_name());
    system_info->set_os_arch(get_os_arch());
    system_info->set_os_description(get_os_description());
    system_info->set_os_version(get_os_version());
    system_info->set_os_vendor(get_os_vendor());
    system_info->set_os_vendor_name(get_os_vendor_name());

    //Hardware info
    system_info->set_cpu_model(get_cpu_model());
    system_info->set_cpu_vendor(get_cpu_vendor());
    system_info->set_cpu_frequency(get_cpu_frequency());
    system_info->set_physical_memory(get_phys_mem());

    //File systems info
    const auto fs_count = get_fs_count();
    for(int i = 0; i < fs_count; i++){
        auto file_system_info = system_info->add_file_system_info();    
        //send onetime info
        file_system_info->set_name(get_fs_name(i));
        file_system_info->set_type((re_common::FileSystemInfo::Type)get_fs_type(i));
        file_system_info->set_size(get_fs_size(i));
    }

    //Network interface info
    const auto interface_count = get_interface_count();
    for(int i = 0; i < interface_count; i++){ 
        if(get_interface_state(i, SystemInfo::InterfaceState::UP)){
            auto interface_info = system_info->add_interface_info();

            //get onetime info
            interface_info->set_name(get_interface_name(i));            
            interface_info->set_type(get_interface_type(i));
            interface_info->set_description(get_interface_description(i));
            interface_info->set_ipv4_addr(get_interface_ipv4(i));
            interface_info->set_ipv6_addr(get_interface_ipv6(i));
            interface_info->set_mac_addr(get_interface_mac(i));
            interface_info->set_speed(get_interface_speed(i));
        }
    }

    
    return system_info;
}

void SystemInfo::Update(){
    std::lock_guard<std::mutex> lock(mutex_);
    auto update_time = UpdateData();
};

int SystemInfo::RegisterListener(){
    std::lock_guard<std::mutex> lock(mutex_);
    return ++listener_count_;
}