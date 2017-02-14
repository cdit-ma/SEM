#include "logprotohandler.h"
#include "tableinsert.h"
#include "sqlitedatabase.h"
#include <functional>
#include "table.h"


#define LOGAN_DECIMAL "DECIMAL"
#define LOGAN_VARCHAR "VARCHAR"
#define LOGAN_INT "INTEGER"

#define LOGAN_TIMEOFDAY "timeofday"
#define LOGAN_HOSTNAME "hostname"
#define LOGAN_COMPONENT_NAME "component_name"
#define LOGAN_COMPONENT_ID "component_id"
#define LOGAN_PORT_NAME "port_name"
#define LOGAN_PORT_ID "port_id"
#define LOGAN_PORT_KIND "port_kind"
#define LOGAN_EVENT "event"
#define LOGAN_MESSAGE_ID "id"
#define LOGAN_NAME "name"


#define LOGAN_SYSTEM_STATUS_TABLE "Hardware_SystemStatus"
#define LOGAN_SYSTEM_INFO_TABLE "Hardware_SystemInfo"
#define LOGAN_CPU_TABLE "Hardware_SystemCPUCoreStatus"
#define LOGAN_FILE_SYSTEM_TABLE "Hardware_FileSystemStatus"
#define LOGAN_FILE_SYSTEM_INFO_TABLE "Hardware_FileSystemInfo"
#define LOGAN_INTERFACE_STATUS_TABLE "Hardware_InterfaceStatus"
#define LOGAN_INTERFACE_INFO_TABLE "Hardware_InterfaceInfo"
#define LOGAN_PROCESS_STATUS_TABLE "Hardware_ProcessStatus"
#define LOGAN_PROCESS_INFO_TABLE "Hardware_ProcessInfo"
#define LOGAN_PORT_EVENT_TABLE "Model_PortEvent"
#define LOGAN_COMPONENT_EVENT_TABLE "Model_ComponentEvent"
#define LOGAN_MESSAGE_EVENT_TABLE "Model_MessageEvent"
#define LOGAN_USER_EVENT_TABLE "Model_UserEvent"

LogProtoHandler::LogProtoHandler(ZMQReceiver* receiver, SQLiteDatabase* database){
    database_ = database;
    receiver_ = receiver;

    receiver_->RegisterNewProto(new SystemStatus());
    receiver_->RegisterNewProto(new re_common::UserEvent());
    receiver_->RegisterNewProto(new re_common::MessageEvent());
    receiver_->RegisterNewProto(new re_common::LifecycleEvent());

    //Set zmqreceiver's callback function
    auto receive_callback = std::bind(&LogProtoHandler::Process, this, std::placeholders::_1);
    receiver_->SetProtoHandlerCallback(receive_callback);

    //Construct all of our tables

    SystemStatusTable();
    SystemInfoTable();
    CpuTable();
    FileSystemTable();
    FileSystemInfoTable();
    InterfaceTable();
    InterfaceInfoTable();
    ProcessTable();
    ProcessInfoTable();
    PortEventTable();
    ComponentEventTable();
    MessageEventTable();
    UserEventTable();
    database_->Flush();
}

void LogProtoHandler::SystemStatusTable(){
    if(table_map_.count(LOGAN_SYSTEM_STATUS_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_SYSTEM_STATUS_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("cpu_utilization", LOGAN_DECIMAL);
    t->AddColumn("phys_mem_utilization", LOGAN_DECIMAL);

    table_map_[LOGAN_SYSTEM_STATUS_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());
}

void LogProtoHandler::SystemInfoTable(){
    if(table_map_.count(LOGAN_SYSTEM_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_SYSTEM_INFO_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
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

    table_map_[LOGAN_SYSTEM_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());
}

void LogProtoHandler::CpuTable(){
    if(table_map_.count(LOGAN_CPU_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_CPU_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("core_id", LOGAN_INT);
    t->AddColumn("core_utilization", LOGAN_DECIMAL);

    table_map_[LOGAN_CPU_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::FileSystemTable(){
    if(table_map_.count(LOGAN_FILE_SYSTEM_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_FILE_SYSTEM_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("utilization", LOGAN_DECIMAL);

    table_map_[LOGAN_FILE_SYSTEM_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::FileSystemInfoTable(){
    if(table_map_.count(LOGAN_FILE_SYSTEM_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_FILE_SYSTEM_INFO_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("type", LOGAN_VARCHAR);
    t->AddColumn("size", LOGAN_INT);

    table_map_[LOGAN_FILE_SYSTEM_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::InterfaceTable(){
    if(table_map_.count(LOGAN_INTERFACE_STATUS_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_INTERFACE_STATUS_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("rx_packets", LOGAN_INT);
    t->AddColumn("rx_bytes", LOGAN_INT);
    t->AddColumn("tx_packets", LOGAN_INT);
    t->AddColumn("tx_bytes", LOGAN_INT);

    table_map_[LOGAN_INTERFACE_STATUS_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::InterfaceInfoTable(){
    if(table_map_.count(LOGAN_INTERFACE_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_INTERFACE_INFO_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("type", LOGAN_VARCHAR);
    t->AddColumn("description", LOGAN_VARCHAR);
    t->AddColumn("ipv4_addr", LOGAN_VARCHAR);
    t->AddColumn("ipv6_addr", LOGAN_VARCHAR);
    t->AddColumn("mac_addr", LOGAN_VARCHAR);
    t->AddColumn("speed", LOGAN_INT);

    table_map_[LOGAN_INTERFACE_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::ProcessTable(){
    if(table_map_.count(LOGAN_PROCESS_STATUS_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_PROCESS_STATUS_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("pid", LOGAN_INT);
    t->AddColumn("core_id", LOGAN_INT);
    t->AddColumn("cpu_utilization", LOGAN_DECIMAL);
    t->AddColumn("phys_mem_utilization", LOGAN_DECIMAL);
    t->AddColumn("thread_count", LOGAN_INT);
    t->AddColumn("disk_read", LOGAN_INT);
    t->AddColumn("disk_written", LOGAN_INT);
    t->AddColumn("disk_total", LOGAN_INT);
    t->AddColumn("state", LOGAN_VARCHAR);

    table_map_[LOGAN_PROCESS_STATUS_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::ProcessInfoTable(){
    if(table_map_.count(LOGAN_PROCESS_INFO_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_PROCESS_INFO_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("pid", LOGAN_INT);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("args", LOGAN_VARCHAR);
    t->AddColumn("start_time", LOGAN_INT);
    
    table_map_[LOGAN_PROCESS_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::PortEventTable(){
    if(table_map_.count(LOGAN_PORT_EVENT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_PORT_EVENT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_EVENT, LOGAN_VARCHAR);
    
    table_map_[LOGAN_PORT_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::ComponentEventTable(){
    if(table_map_.count(LOGAN_COMPONENT_EVENT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_COMPONENT_EVENT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_EVENT, LOGAN_VARCHAR);
    
    table_map_[LOGAN_COMPONENT_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::MessageEventTable(){
    if(table_map_.count(LOGAN_MESSAGE_EVENT_TABLE)){
        return;
    }
    Table* t = new Table(database_, LOGAN_MESSAGE_EVENT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_KIND, LOGAN_VARCHAR);
    
    table_map_[LOGAN_MESSAGE_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::UserEventTable(){
    if(table_map_.count(LOGAN_USER_EVENT_TABLE)){
        return;
    }
    Table* t = new Table(database_, LOGAN_USER_EVENT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn("message", LOGAN_VARCHAR);
    t->AddColumn("type", LOGAN_VARCHAR);
    
    table_map_[LOGAN_USER_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}


void LogProtoHandler::Process(google::protobuf::MessageLite* message){
    //Ordered by frequency
    if(static_cast<SystemStatus*>(message)){
        ProcessSystemStatus((SystemStatus*)message);
    }
    else if(static_cast<re_common::LifecycleEvent*>(message)){
        ProcessLifecycleEvent((re_common::LifecycleEvent*)message);
    }
    else if(static_cast<re_common::MessageEvent*>(message)){
        ProcessMessageEvent((re_common::MessageEvent*)message);
    }
    else if(static_cast<re_common::UserEvent*>(message)){
        ProcessUserEvent((re_common::UserEvent*)message);
    }
    else{
        std::cerr << "Cannot Process Message of Type: '" << ((google::protobuf::Message*)message)->GetTypeName() << "'" << std::endl;
    }

    if(message){
        //Free Memory1
        delete message;
    }
}

void LogProtoHandler::ProcessSystemStatus(SystemStatus* status){
    auto stmt = table_map_[LOGAN_SYSTEM_STATUS_TABLE]->get_insert_statement();

    std::string hostname = status->hostname().c_str();
    int message_id = status->message_id();
    double timestamp = status->timestamp();
    
    stmt.BindString(LOGAN_HOSTNAME, hostname);
    stmt.BindInt(LOGAN_MESSAGE_ID, message_id);
    stmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
    stmt.BindDouble("cpu_utilization", status->cpu_utilization());
    stmt.BindDouble("phys_mem_utilization", status->phys_mem_utilization());
    database_->QueueSqlStatement(stmt.get_statement());


    if(status->has_info() != 0){
        auto infostmt = table_map_[LOGAN_SYSTEM_INFO_TABLE]->get_insert_statement();
        infostmt.BindString(LOGAN_HOSTNAME, hostname);
        infostmt.BindInt(LOGAN_MESSAGE_ID, message_id);
        infostmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        
        SystemStatus::SystemInfo info = status->info();
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
        database_->QueueSqlStatement(infostmt.get_statement());
    }


    for(int i = 0; i < status->cpu_core_utilization_size(); i++){
        auto cpustmt = table_map_[LOGAN_CPU_TABLE]->get_insert_statement();        
        cpustmt.BindString(LOGAN_HOSTNAME, hostname);
        cpustmt.BindInt(LOGAN_MESSAGE_ID, message_id);
        cpustmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        cpustmt.BindInt("core_id", i);
        cpustmt.BindDouble("core_utilization", status->cpu_core_utilization(i));
        database_->QueueSqlStatement(cpustmt.get_statement());
    }

    for(int i = 0; i < status->processes_size(); i++){
        auto procstmt = table_map_[LOGAN_PROCESS_STATUS_TABLE]->get_insert_statement();
        ProcessStatus proc = status->processes(i);

        if(proc.has_info()){
            auto procinfo = table_map_[LOGAN_PROCESS_INFO_TABLE]->get_insert_statement();
            procinfo.BindString(LOGAN_HOSTNAME, hostname);
            procinfo.BindInt(LOGAN_MESSAGE_ID, message_id);
            procinfo.BindDouble(LOGAN_TIMEOFDAY, timestamp);
            procinfo.BindInt("pid", proc.pid());
            procinfo.BindString(LOGAN_NAME, proc.info().name());
            procinfo.BindString("args", proc.info().args());
            procinfo.BindInt("start_time", proc.info().start_time());
            database_->QueueSqlStatement(procinfo.get_statement());
        }
        procstmt.BindString(LOGAN_HOSTNAME, hostname);
        procstmt.BindInt(LOGAN_MESSAGE_ID, message_id);
        procstmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        procstmt.BindInt("pid", proc.pid());
        procstmt.BindInt("core_id", proc.cpu_core_id());
        procstmt.BindDouble("cpu_utilization", proc.cpu_utilization());
        procstmt.BindDouble("phys_mem_utilization", proc.phys_mem_utilization());
        procstmt.BindInt("thread_count", proc.thread_count());
        procstmt.BindInt("disk_read", proc.disk_read());
        procstmt.BindInt("disk_written", proc.disk_written());
        procstmt.BindInt("disk_total", proc.disk_total());
        procstmt.BindString("state", ProcessStatus::State_Name(proc.state()));
        database_->QueueSqlStatement(procstmt.get_statement());
    }

    for(int i = 0; i < status->interfaces_size(); i++){
        auto ifstatement = table_map_[LOGAN_INTERFACE_STATUS_TABLE]->get_insert_statement();        
        InterfaceStatus ifstat = status->interfaces(i);

        if(ifstat.has_info()){
            auto ifinfo = table_map_[LOGAN_INTERFACE_INFO_TABLE]->get_insert_statement();        
            
            ifinfo.BindString(LOGAN_HOSTNAME, hostname);
            ifinfo.BindInt(LOGAN_MESSAGE_ID, message_id);
            ifinfo.BindDouble(LOGAN_TIMEOFDAY, timestamp);

            ifinfo.BindString(LOGAN_NAME, ifstat.name());
            ifinfo.BindString("type", ifstat.info().type());
            ifinfo.BindString("description", ifstat.info().description());
            ifinfo.BindString("ipv4_addr", ifstat.info().ipv4_addr());
            ifinfo.BindString("ipv6_addr", ifstat.info().ipv6_addr());
            ifinfo.BindString("mac_addr", ifstat.info().mac_addr());
            ifinfo.BindInt("speed", ifstat.info().speed());

            database_->QueueSqlStatement(ifinfo.get_statement());
        }

        ifstatement.BindString(LOGAN_HOSTNAME, hostname);
        ifstatement.BindInt(LOGAN_MESSAGE_ID, message_id);
        ifstatement.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        ifstatement.BindString(LOGAN_NAME, ifstat.name());
        ifstatement.BindInt("rx_packets", ifstat.rx_packets());
        ifstatement.BindInt("rx_bytes", ifstat.rx_bytes());
        ifstatement.BindInt("tx_packets", ifstat.tx_packets());
        ifstatement.BindInt("tx_bytes", ifstat.tx_bytes());
        database_->QueueSqlStatement(ifstatement.get_statement());
    }

    for(int i = 0; i < status->file_systems_size(); i++){
        auto fsstatement = table_map_[LOGAN_FILE_SYSTEM_TABLE]->get_insert_statement();                
        FileSystemStatus fss = status->file_systems(i);

        if(fss.has_info()){
            auto fsinfo = table_map_[LOGAN_FILE_SYSTEM_INFO_TABLE]->get_insert_statement();

            fsinfo.BindString(LOGAN_HOSTNAME, hostname);
            fsinfo.BindInt(LOGAN_MESSAGE_ID, message_id);
            fsinfo.BindDouble(LOGAN_TIMEOFDAY, timestamp);                        
            fsinfo.BindString(LOGAN_NAME, fss.name());
            fsinfo.BindString("type", FileSystemStatus::FileSystemInfo::Type_Name(fss.info().type()));
            fsinfo.BindInt("size", fss.info().size());
            database_->QueueSqlStatement(fsinfo.get_statement());
        }

        fsstatement.BindString(LOGAN_HOSTNAME, hostname);
        fsstatement.BindInt(LOGAN_MESSAGE_ID, message_id);
        fsstatement.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        fsstatement.BindString(LOGAN_NAME, fss.name());
        fsstatement.BindDouble("utilization", fss.utilization());
        database_->QueueSqlStatement(fsstatement.get_statement());
    }
}

void LogProtoHandler::ProcessLifecycleEvent(re_common::LifecycleEvent* event){
    if(event->has_port() && event->has_component()){
        //Process port event
        //Insert test Statements
        auto ins = table_map_[LOGAN_PORT_EVENT_TABLE]->get_insert_statement();
        ins.BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
        ins.BindString(LOGAN_HOSTNAME, event->info().hostname());
        ins.BindString(LOGAN_COMPONENT_NAME, event->component().name());
        ins.BindString(LOGAN_COMPONENT_ID, event->component().id());
        ins.BindString(LOGAN_PORT_NAME, event->port().name());
        ins.BindString(LOGAN_PORT_ID, event->port().id());
        ins.BindString(LOGAN_EVENT, re_common::LifecycleEvent::Type_Name(event->type()));

        database_->QueueSqlStatement(ins.get_statement());
    }

    else if(event->has_component()){
            auto ins = table_map_[LOGAN_COMPONENT_EVENT_TABLE]->get_insert_statement();
            ins.BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
            ins.BindString(LOGAN_HOSTNAME, event->info().hostname());
            ins.BindString(LOGAN_COMPONENT_NAME, event->component().name());
            ins.BindString(LOGAN_COMPONENT_ID, event->component().id());
            database_->QueueSqlStatement(ins.get_statement());
    }
}

void LogProtoHandler::ProcessMessageEvent(re_common::MessageEvent* event){
    if(event->has_port() && event->has_component()){
        //Process port event
        auto ins = table_map_[LOGAN_MESSAGE_EVENT_TABLE]->get_insert_statement();
        ins.BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
        ins.BindString(LOGAN_HOSTNAME, event->info().hostname());
        ins.BindString(LOGAN_COMPONENT_NAME, event->component().name());
        ins.BindString(LOGAN_COMPONENT_ID, event->component().id());
        ins.BindString(LOGAN_PORT_NAME, event->port().name());
        ins.BindString(LOGAN_PORT_ID, event->port().id());
        ins.BindString(LOGAN_PORT_KIND, re_common::Port::PortKind_Name(event->port().kind()));

        database_->QueueSqlStatement(ins.get_statement());
    }
}

void LogProtoHandler::ProcessUserEvent(re_common::UserEvent* event){

    auto ins = table_map_[LOGAN_USER_EVENT_TABLE]->get_insert_statement();
    ins.BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
    ins.BindString(LOGAN_HOSTNAME, event->info().hostname());
    ins.BindString(LOGAN_COMPONENT_NAME, event->component().name());
    ins.BindString(LOGAN_COMPONENT_ID, event->component().id());
    ins.BindString("message", event->message());
    ins.BindString("type", re_common::UserEvent::Type_Name(event->type()));
    database_->QueueSqlStatement(ins.get_statement());
    
}