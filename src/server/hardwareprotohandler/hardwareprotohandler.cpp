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

#include "hardwareprotohandler.h"

#include <functional>
#include <chrono>

#include "../sqlitedatabase.h"
#include "../table.h"
#include "../tableinsert.h"

#include <re_common/zmq/protoreceiver/protoreceiver.h>
#include <re_common/proto/systemstatus/systemstatus.pb.h>

//Type names
const std::string LOGAN_DECIMAL = "DECIMAL";
const std::string LOGAN_VARCHAR = "VARCHAR";
const std::string LOGAN_INT = "INTEGER";

//Common column names
const std::string LOGAN_TIMEOFDAY = "timeofday";
const std::string LOGAN_HOSTNAME = "hostname";
const std::string LOGAN_MESSAGE_ID = "id";
const std::string LOGAN_NAME = "name";
const std::string LOGAN_TYPE = "type";

//Hardware table names
const std::string LOGAN_SYSTEM_STATUS_TABLE = "Hardware_SystemStatus";
const std::string LOGAN_SYSTEM_INFO_TABLE = "Hardware_SystemInfo";
const std::string LOGAN_CPU_TABLE = "Hardware_SystemCPUCoreStatus";
const std::string LOGAN_FILE_SYSTEM_TABLE = "Hardware_FileSystemStatus";
const std::string LOGAN_FILE_SYSTEM_INFO_TABLE = "Hardware_FileSystemInfo";
const std::string LOGAN_INTERFACE_STATUS_TABLE = "Hardware_InterfaceStatus";
const std::string LOGAN_INTERFACE_INFO_TABLE = "Hardware_InterfaceInfo";
const std::string LOGAN_PROCESS_STATUS_TABLE = "Hardware_ProcessStatus";
const std::string LOGAN_PROCESS_INFO_TABLE = "Hardware_ProcessInfo";

HardwareProtoHandler::HardwareProtoHandler(SQLiteDatabase& database):
    ProtoHandler(),
    database_(database)
{
    //Create the relevant tables
    CreateSystemStatusTable();
    CreateSystemInfoTable();
    CreateCpuTable();
    CreateFileSystemTable();
    CreateFileSystemInfoTable();
    CreateInterfaceTable();
    CreateInterfaceInfoTable();
    CreateProcessTable();
    CreateProcessInfoTable();
}


void HardwareProtoHandler::BindCallbacks(zmq::ProtoReceiver& receiver){
    //Register call back functions and type with zmqreceiver
    receiver.RegisterProtoCallback<re_common::SystemStatus>(std::bind(&HardwareProtoHandler::ProcessSystemStatus, this, std::placeholders::_1));
    receiver.RegisterProtoCallback<re_common::SystemInfo>(std::bind(&HardwareProtoHandler::ProcessOneTimeSystemInfo, this, std::placeholders::_1));
}

void HardwareProtoHandler::CreateSystemStatusTable(){
    if(table_map_.count(LOGAN_SYSTEM_STATUS_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_SYSTEM_STATUS_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn("cpu_utilization", LOGAN_DECIMAL);
    t->AddColumn("phys_mem_utilization", LOGAN_DECIMAL);

    table_map_[LOGAN_SYSTEM_STATUS_TABLE] = t;
    t->Finalize();
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateSystemInfoTable(){
    if(table_map_.count(LOGAN_SYSTEM_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_SYSTEM_INFO_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn("os_name", LOGAN_VARCHAR);
    t->AddColumn("os_arch", LOGAN_VARCHAR);
    t->AddColumn("os_description", LOGAN_VARCHAR);
    t->AddColumn("os_version", LOGAN_VARCHAR);
    t->AddColumn("os_vendor", LOGAN_VARCHAR);
    t->AddColumn("os_vendor_name", LOGAN_VARCHAR);
    t->AddColumn("cpu_model", LOGAN_VARCHAR);
    t->AddColumn("cpu_vendor", LOGAN_VARCHAR);
    t->AddColumn("cpu_frequency", LOGAN_INT);
    t->AddColumn("physical_memory", LOGAN_INT);
    t->Finalize();

    table_map_[LOGAN_SYSTEM_INFO_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateCpuTable(){
    if(table_map_.count(LOGAN_CPU_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_CPU_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn("core_id", LOGAN_INT);
    t->AddColumn("core_utilization", LOGAN_DECIMAL);
    t->Finalize();

    table_map_[LOGAN_CPU_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateFileSystemTable(){
    if(table_map_.count(LOGAN_FILE_SYSTEM_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_FILE_SYSTEM_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("utilization", LOGAN_DECIMAL);
    t->Finalize();

    table_map_[LOGAN_FILE_SYSTEM_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateFileSystemInfoTable(){
    if(table_map_.count(LOGAN_FILE_SYSTEM_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_FILE_SYSTEM_INFO_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_TYPE, LOGAN_VARCHAR);
    t->AddColumn("size", LOGAN_INT);
    t->Finalize();

    table_map_[LOGAN_FILE_SYSTEM_INFO_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateInterfaceTable(){
    if(table_map_.count(LOGAN_INTERFACE_STATUS_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_INTERFACE_STATUS_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("rx_packets", LOGAN_INT);
    t->AddColumn("rx_bytes", LOGAN_INT);
    t->AddColumn("tx_packets", LOGAN_INT);
    t->AddColumn("tx_bytes", LOGAN_INT);
    t->Finalize();

    table_map_[LOGAN_INTERFACE_STATUS_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateInterfaceInfoTable(){
    if(table_map_.count(LOGAN_INTERFACE_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_INTERFACE_INFO_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_TYPE, LOGAN_VARCHAR);
    t->AddColumn("description", LOGAN_VARCHAR);
    t->AddColumn("ipv4_addr", LOGAN_VARCHAR);
    t->AddColumn("ipv6_addr", LOGAN_VARCHAR);
    t->AddColumn("mac_addr", LOGAN_VARCHAR);
    t->AddColumn("speed", LOGAN_INT);
    t->Finalize();

    table_map_[LOGAN_INTERFACE_INFO_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateProcessTable(){
    if(table_map_.count(LOGAN_PROCESS_STATUS_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_PROCESS_STATUS_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn("pid", LOGAN_INT);
    t->AddColumn("name", LOGAN_VARCHAR);
    t->AddColumn("core_id", LOGAN_INT);
    t->AddColumn("cpu_utilization", LOGAN_DECIMAL);
    t->AddColumn("phys_mem_utilization", LOGAN_DECIMAL);
    t->AddColumn("thread_count", LOGAN_INT);
    t->AddColumn("disk_read", LOGAN_INT);
    t->AddColumn("disk_written", LOGAN_INT);
    t->AddColumn("disk_total", LOGAN_INT);
    t->AddColumn("state", LOGAN_VARCHAR);
    t->Finalize();

    table_map_[LOGAN_PROCESS_STATUS_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}

void HardwareProtoHandler::CreateProcessInfoTable(){
    if(table_map_.count(LOGAN_PROCESS_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_PROCESS_INFO_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn("pid", LOGAN_INT);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("args", LOGAN_VARCHAR);
    t->AddColumn("start_time", LOGAN_INT);
    t->Finalize();

    table_map_[LOGAN_PROCESS_INFO_TABLE] = t;
    database_.QueueSqlStatement(t->get_table_construct_statement());
}


void HardwareProtoHandler::ProcessSystemStatus(const re_common::SystemStatus& status){
    auto stmt = table_map_[LOGAN_SYSTEM_STATUS_TABLE]->get_insert_statement();

    std::string hostname = status.hostname();
    int message_id = (int)(status.message_id());
    double timestamp = status.timestamp();

    stmt.BindString(LOGAN_HOSTNAME, hostname);
    stmt.BindInt(LOGAN_MESSAGE_ID, message_id);
    stmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
    stmt.BindDouble("cpu_utilization", status.cpu_utilization());
    stmt.BindDouble("phys_mem_utilization", status.phys_mem_utilization());
    database_.QueueSqlStatement(stmt.get_statement());


    for(int i = 0; i < status.cpu_core_utilization_size(); i++){
        auto cpustmt = table_map_[LOGAN_CPU_TABLE]->get_insert_statement();
        cpustmt.BindString(LOGAN_HOSTNAME, hostname);
        cpustmt.BindInt(LOGAN_MESSAGE_ID, message_id);
        cpustmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        cpustmt.BindInt("core_id", i);
        cpustmt.BindDouble("core_utilization", status.cpu_core_utilization(i));
        database_.QueueSqlStatement(cpustmt.get_statement());
    }

    for(int i = 0; i < status.processes_size(); i++){
        auto procstmt = table_map_[LOGAN_PROCESS_STATUS_TABLE]->get_insert_statement();
        re_common::ProcessStatus proc = status.processes(i);

        procstmt.BindString(LOGAN_HOSTNAME, hostname);
        procstmt.BindInt(LOGAN_MESSAGE_ID, message_id);
        procstmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        procstmt.BindInt("pid", proc.pid());
        procstmt.BindString("name", proc.name());
        procstmt.BindInt("core_id", proc.cpu_core_id());
        procstmt.BindDouble("cpu_utilization", proc.cpu_utilization());
        procstmt.BindDouble("phys_mem_utilization", proc.phys_mem_utilization());
        procstmt.BindInt("thread_count", proc.thread_count());
        procstmt.BindInt("disk_read", (int)(proc.disk_read()));
        procstmt.BindInt("disk_written", (int)(proc.disk_written()));
        procstmt.BindInt("disk_total", (int)(proc.disk_total()));
        procstmt.BindString("state", re_common::ProcessStatus::State_Name(proc.state()));
        database_.QueueSqlStatement(procstmt.get_statement());
    }

    for(int i = 0; i < status.interfaces_size(); i++){
        auto ifstatement = table_map_[LOGAN_INTERFACE_STATUS_TABLE]->get_insert_statement();
        re_common::InterfaceStatus ifstat = status.interfaces(i);

        ifstatement.BindString(LOGAN_HOSTNAME, hostname);
        ifstatement.BindInt(LOGAN_MESSAGE_ID, message_id);
        ifstatement.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        ifstatement.BindString(LOGAN_NAME, ifstat.name());
        ifstatement.BindInt("rx_packets", (int)(ifstat.rx_packets()));
        ifstatement.BindInt("rx_bytes", (int)(ifstat.rx_bytes()));
        ifstatement.BindInt("tx_packets", (int)(ifstat.tx_packets()));
        ifstatement.BindInt("tx_bytes", (int)(ifstat.tx_bytes()));
        database_.QueueSqlStatement(ifstatement.get_statement());
    }

    for(int i = 0; i < status.file_systems_size(); i++){
        auto fsstatement = table_map_[LOGAN_FILE_SYSTEM_TABLE]->get_insert_statement();
        re_common::FileSystemStatus fss = status.file_systems(i);

        fsstatement.BindString(LOGAN_HOSTNAME, hostname);
        fsstatement.BindInt(LOGAN_MESSAGE_ID, message_id);
        fsstatement.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        fsstatement.BindString(LOGAN_NAME, fss.name());
        fsstatement.BindDouble("utilization", fss.utilization());
        database_.QueueSqlStatement(fsstatement.get_statement());
    }

    for(int i = 0; i < status.process_info_size(); i++){
        re_common::ProcessInfo proc_info = status.process_info(i);
        if(proc_info.pid() != 0){
            auto proc_insert = table_map_[LOGAN_PROCESS_INFO_TABLE]->get_insert_statement();
            proc_insert.BindString(LOGAN_HOSTNAME, hostname);
            proc_insert.BindInt(LOGAN_MESSAGE_ID, message_id);
            proc_insert.BindDouble(LOGAN_TIMEOFDAY, timestamp);
            proc_insert.BindInt("pid", proc_info.pid());
            proc_insert.BindString(LOGAN_NAME, proc_info.name());
            proc_insert.BindString("args", proc_info.args());
            proc_insert.BindDouble("start_time", proc_info.start_time());
            database_.QueueSqlStatement(proc_insert.get_statement());
        }
    }
}

void HardwareProtoHandler::ProcessOneTimeSystemInfo(const re_common::SystemInfo& info){
    std::string hostname = info.hostname();
    //Check if we have this node info already
    if(registered_nodes_.find(hostname) != registered_nodes_.end()){
        return;
    }

    int message_id = (int)(info.message_id());
    double timestamp = info.timestamp();

    auto infostmt = table_map_[LOGAN_SYSTEM_INFO_TABLE]->get_insert_statement();
    infostmt.BindString(LOGAN_HOSTNAME, hostname);
    infostmt.BindInt(LOGAN_MESSAGE_ID, message_id);
    infostmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);

    infostmt.BindString("os_name", info.os_name());
    infostmt.BindString("os_arch", info.os_arch());
    infostmt.BindString("os_description", info.os_description());
    infostmt.BindString("os_version", info.os_version());
    infostmt.BindString("os_vendor", info.os_vendor());
    infostmt.BindString("os_vendor_name", info.os_vendor_name());
    infostmt.BindString("cpu_model", info.cpu_model());
    infostmt.BindString("cpu_vendor", info.cpu_vendor());
    infostmt.BindInt("cpu_frequency", info.cpu_frequency());
    infostmt.BindInt("physical_memory", info.physical_memory());
    database_.QueueSqlStatement(infostmt.get_statement());

    for(int i = 0; i < info.file_system_info_size(); i++){
        re_common::FileSystemInfo fsi = info.file_system_info(i);

        auto fsinfo = table_map_[LOGAN_FILE_SYSTEM_INFO_TABLE]->get_insert_statement();
        fsinfo.BindString(LOGAN_HOSTNAME, hostname);
        fsinfo.BindInt(LOGAN_MESSAGE_ID, message_id);
        fsinfo.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        fsinfo.BindString(LOGAN_NAME, fsi.name());
        fsinfo.BindString(LOGAN_TYPE, re_common::FileSystemInfo::Type_Name(fsi.type()));
        fsinfo.BindInt("size", (int)(fsi.size()));
        database_.QueueSqlStatement(fsinfo.get_statement());
    }

    for(int i = 0; i < info.interface_info_size(); i++){
        re_common::InterfaceInfo if_info = info.interface_info(i);

        auto if_insert = table_map_[LOGAN_INTERFACE_INFO_TABLE]->get_insert_statement();

        if_insert.BindString(LOGAN_HOSTNAME, hostname);
        if_insert.BindInt(LOGAN_MESSAGE_ID, message_id);
        if_insert.BindDouble(LOGAN_TIMEOFDAY, timestamp);

        if_insert.BindString(LOGAN_NAME, if_info.name());
        if_insert.BindString("type", if_info.type());
        if_insert.BindString("description", if_info.description());
        if_insert.BindString("ipv4_addr", if_info.ipv4_addr());
        if_insert.BindString("ipv6_addr", if_info.ipv6_addr());
        if_insert.BindString("mac_addr", if_info.mac_addr());
        if_insert.BindInt("speed", (int)(if_info.speed()));

        database_.QueueSqlStatement(if_insert.get_statement());
    }
}
