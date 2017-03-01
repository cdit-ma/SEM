#include "logprotohandler.h"

#include <functional>
#include <chrono>

#include "sqlitedatabase.h"
#include "table.h"
#include "tableinsert.h"

#include "../re_common/proto/modelevent/modelevent.pb.h"
#include "../re_common/proto/systemstatus/systemstatus.pb.h"
#include "../re_common/zmq/protoreceiver/protoreceiver.h"

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
#define LOGAN_WORKLOAD_EVENT_TABLE "Model_WorkloadEvent"
#define LOGAN_CLIENT_TABLE "Clients"

LogProtoHandler::LogProtoHandler(std::string database_file, std::vector<std::string> client_addresses){
    //Construct a Receiver to connect to the clients
	receiver_ = new zmq::ProtoReceiver();
	//Construct a SQLite database responsible for writing the received messages to the 
	database_ = new SQLiteDatabase(database_file);

    //Register call back functions and type with zmqreceiver
    auto ss_callback = std::bind(&LogProtoHandler::ProcessSystemStatus, this, std::placeholders::_1);
    receiver_->RegisterNewProto(SystemStatus::default_instance(), ss_callback);

    auto si_callback = std::bind(&LogProtoHandler::ProcessOneTimeSystemInfo, this, std::placeholders::_1);
    receiver_->RegisterNewProto(OneTimeSystemInfo::default_instance(), si_callback);

    auto ue_callback = std::bind(&LogProtoHandler::ProcessUserEvent, this, std::placeholders::_1);
    receiver_->RegisterNewProto(re_common::UserEvent::default_instance(), ue_callback);

    auto me_callback = std::bind(&LogProtoHandler::ProcessMessageEvent, this, std::placeholders::_1);
    receiver_->RegisterNewProto(re_common::MessageEvent::default_instance(), me_callback);

    auto le_callback = std::bind(&LogProtoHandler::ProcessLifecycleEvent, this, std::placeholders::_1);
    receiver_->RegisterNewProto(re_common::LifecycleEvent::default_instance(), le_callback);

    for(auto c : client_addresses){
        receiver_->Connect(c);
    }
    
    //Now that we have registered our callbacks we should start.
    receiver_->Start();

    //Construct all of our tables

    CreateSystemStatusTable();
    CreateSystemInfoTable();
    CreateCpuTable();
    CreateFileSystemTable();
    CreateFileSystemInfoTable();
    CreateInterfaceTable();
    CreateInterfaceInfoTable();
    CreateProcessTable();
    CreateProcessInfoTable();
    CreatePortEventTable();
    CreateComponentEventTable();
    CreateMessageEventTable();
    CreateUserEventTable();
    CreateWorkloadEventTable();
    CreateClientTable();
    std::cout << "# Constructing #" << table_map_.size() << " Tables." << std::endl;
    database_->BlockingFlush();
    std::cout << "# Constructed." << std::endl;
    
}

LogProtoHandler::~LogProtoHandler(){
    //Destroy the receiver and database
    delete receiver_;
    delete database_;

    auto itr = table_map_.begin();
    while(itr != table_map_.end()){
        delete itr->second;
        itr = table_map_.erase(itr);
    }
}

void LogProtoHandler::CreateSystemStatusTable(){
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
    t->Finalize();
    database_->QueueSqlStatement(t->get_table_construct_statement());
}

void LogProtoHandler::CreateSystemInfoTable(){
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
    t->Finalize();

    table_map_[LOGAN_SYSTEM_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());
}

void LogProtoHandler::CreateCpuTable(){
    if(table_map_.count(LOGAN_CPU_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_CPU_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("core_id", LOGAN_INT);
    t->AddColumn("core_utilization", LOGAN_DECIMAL);
    t->Finalize();

    table_map_[LOGAN_CPU_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateFileSystemTable(){
    if(table_map_.count(LOGAN_FILE_SYSTEM_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_FILE_SYSTEM_TABLE);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("utilization", LOGAN_DECIMAL);
    t->Finalize();

    table_map_[LOGAN_FILE_SYSTEM_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateFileSystemInfoTable(){
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
    t->Finalize();

    table_map_[LOGAN_FILE_SYSTEM_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateInterfaceTable(){
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
    t->Finalize();

    table_map_[LOGAN_INTERFACE_STATUS_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateInterfaceInfoTable(){
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
    t->Finalize();

    table_map_[LOGAN_INTERFACE_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateProcessTable(){
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
    t->Finalize();

    table_map_[LOGAN_PROCESS_STATUS_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateProcessInfoTable(){
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
    t->Finalize();
    
    table_map_[LOGAN_PROCESS_INFO_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreatePortEventTable(){
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
    t->Finalize();
    
    table_map_[LOGAN_PORT_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateComponentEventTable(){
    if(table_map_.count(LOGAN_COMPONENT_EVENT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_COMPONENT_EVENT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_EVENT, LOGAN_VARCHAR);
    t->Finalize();
    
    table_map_[LOGAN_COMPONENT_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateMessageEventTable(){
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
    t->Finalize();
    
    table_map_[LOGAN_MESSAGE_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());    
}

void LogProtoHandler::CreateUserEventTable(){
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
    t->Finalize();
    
    table_map_[LOGAN_USER_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());
}

void LogProtoHandler::CreateWorkloadEventTable(){
    if(table_map_.count(LOGAN_WORKLOAD_EVENT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_WORKLOAD_EVENT_TABLE);
    //Info
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn("event_type", LOGAN_VARCHAR);
    t->AddColumn("workload_implementation_id", LOGAN_VARCHAR);
    t->AddColumn("function_name", LOGAN_VARCHAR);
    t->AddColumn("function_library", LOGAN_VARCHAR);

    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn("type", LOGAN_VARCHAR);

    t->AddColumn("description", LOGAN_VARCHAR);
    t->Finalize();
    table_map_[LOGAN_WORKLOAD_EVENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());
}

void LogProtoHandler::CreateClientTable(){
    if(table_map_.count(LOGAN_CLIENT_TABLE)){
        return;
    }

    Table* t = new Table(database_, LOGAN_CLIENT_TABLE);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("endpoint", LOGAN_VARCHAR);
    t->Finalize();
    table_map_[LOGAN_CLIENT_TABLE] = t;
    database_->QueueSqlStatement(t->get_table_construct_statement());
}

void LogProtoHandler::ProcessSystemStatus(google::protobuf::MessageLite* ml){
    SystemStatus* status = (SystemStatus*)ml;
    auto stmt = table_map_[LOGAN_SYSTEM_STATUS_TABLE]->get_insert_statement();

    std::string hostname = status->hostname();
    int message_id = (int)(status->message_id());
    double timestamp = status->timestamp();
    
    stmt.BindString(LOGAN_HOSTNAME, hostname);
    stmt.BindInt(LOGAN_MESSAGE_ID, message_id);
    stmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
    stmt.BindDouble("cpu_utilization", status->cpu_utilization());
    stmt.BindDouble("phys_mem_utilization", status->phys_mem_utilization());
    database_->QueueSqlStatement(stmt.get_statement());

   
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

        procstmt.BindString(LOGAN_HOSTNAME, hostname);
        procstmt.BindInt(LOGAN_MESSAGE_ID, message_id);
        procstmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        procstmt.BindInt("pid", proc.pid());
        procstmt.BindInt("core_id", proc.cpu_core_id());
        procstmt.BindDouble("cpu_utilization", proc.cpu_utilization());
        procstmt.BindDouble("phys_mem_utilization", proc.phys_mem_utilization());
        procstmt.BindInt("thread_count", proc.thread_count());
        procstmt.BindInt("disk_read", (int)(proc.disk_read()));
        procstmt.BindInt("disk_written", (int)(proc.disk_written()));
        procstmt.BindInt("disk_total", (int)(proc.disk_total()));
        procstmt.BindString("state", ProcessStatus::State_Name(proc.state()));
        database_->QueueSqlStatement(procstmt.get_statement());
    }

    for(int i = 0; i < status->interfaces_size(); i++){
        auto ifstatement = table_map_[LOGAN_INTERFACE_STATUS_TABLE]->get_insert_statement();        
        InterfaceStatus ifstat = status->interfaces(i);

        ifstatement.BindString(LOGAN_HOSTNAME, hostname);
        ifstatement.BindInt(LOGAN_MESSAGE_ID, message_id);
        ifstatement.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        ifstatement.BindString(LOGAN_NAME, ifstat.name());
        ifstatement.BindInt("rx_packets", (int)(ifstat.rx_packets()));
        ifstatement.BindInt("rx_bytes", (int)(ifstat.rx_bytes()));
        ifstatement.BindInt("tx_packets", (int)(ifstat.tx_packets()));
        ifstatement.BindInt("tx_bytes", (int)(ifstat.tx_bytes()));
        database_->QueueSqlStatement(ifstatement.get_statement());
    }

    for(int i = 0; i < status->file_systems_size(); i++){
        auto fsstatement = table_map_[LOGAN_FILE_SYSTEM_TABLE]->get_insert_statement();                
        FileSystemStatus fss = status->file_systems(i);

        fsstatement.BindString(LOGAN_HOSTNAME, hostname);
        fsstatement.BindInt(LOGAN_MESSAGE_ID, message_id);
        fsstatement.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        fsstatement.BindString(LOGAN_NAME, fss.name());
        fsstatement.BindDouble("utilization", fss.utilization());
        database_->QueueSqlStatement(fsstatement.get_statement());
    }

    /*for(int i = 0; i < info->process_info_size(); i++){
        ProcessInfo proc_info = info->process_info(i);

        auto proc_insert = table_map_[LOGAN_PROCESS_INFO_TABLE]->get_insert_statement();
        proc_insert.BindString(LOGAN_HOSTNAME, hostname);
        proc_insert.BindInt(LOGAN_MESSAGE_ID, message_id);
        proc_insert.BindDouble(LOGAN_TIMEOFDAY, timestamp);
        proc_insert.BindInt("pid", proc_info.pid());
        proc_insert.BindString(LOGAN_NAME, proc_info.name());
        proc_insert.BindString("args", proc_info.args());
        proc_insert.BindDouble("start_time", proc_info.start_time());
        database_->QueueSqlStatement(proc_insert.get_statement());
    }*/
}

void LogProtoHandler::ProcessOneTimeSystemInfo(google::protobuf::MessageLite* message){
    OneTimeSystemInfo* info = (OneTimeSystemInfo*)message;

    std::string hostname = info->hostname();
    //Check if we have this node info already
    if(registered_nodes_.find(hostname) != registered_nodes_.end()){
        return;
    }
    else{
        //Haven't seen this node before, add to set and record in db
        registered_nodes_.insert(hostname);
        auto ins = table_map_[LOGAN_CLIENT_TABLE]->get_insert_statement();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        ins.BindDouble(LOGAN_TIMEOFDAY, time.count()/1000.0);
        ins.BindString("endpoint", hostname);
        database_->QueueSqlStatement(ins.get_statement());
    }

    int message_id = (int)(info->message_id());
    double timestamp = info->timestamp();

    auto infostmt = table_map_[LOGAN_SYSTEM_INFO_TABLE]->get_insert_statement();
    infostmt.BindString(LOGAN_HOSTNAME, hostname);
    infostmt.BindInt(LOGAN_MESSAGE_ID, message_id);
    infostmt.BindDouble(LOGAN_TIMEOFDAY, timestamp);

    infostmt.BindString("os_name", info->os_name());
    infostmt.BindString("os_arch", info->os_arch());
    infostmt.BindString("os_description", info->os_description());
    infostmt.BindString("os_version", info->os_version());
    infostmt.BindString("os_vendor", info->os_vendor());
    infostmt.BindString("os_vendor_name", info->os_vendor_name());
    infostmt.BindString("cpu_model", info->cpu_model());
    infostmt.BindString("cpu_vendor", info->cpu_vendor());
    infostmt.BindInt("cpu_frequency", info->cpu_frequency());
    infostmt.BindInt("physical_memory", info->physical_memory());
    database_->QueueSqlStatement(infostmt.get_statement());

    for(int i = 0; i < info->file_system_info_size(); i++){
        FileSystemInfo fsi = info->file_system_info(i);

        auto fsinfo = table_map_[LOGAN_FILE_SYSTEM_INFO_TABLE]->get_insert_statement();
        fsinfo.BindString(LOGAN_HOSTNAME, hostname);
        fsinfo.BindInt(LOGAN_MESSAGE_ID, message_id);
        fsinfo.BindDouble(LOGAN_TIMEOFDAY, timestamp);                        
        fsinfo.BindString(LOGAN_NAME, fsi.name());
        fsinfo.BindString("type", FileSystemInfo::Type_Name(fsi.type()));
        fsinfo.BindInt("size", (int)(fsi.size()));
        database_->QueueSqlStatement(fsinfo.get_statement());
    }

    for(int i = 0; i < info->interface_info_size(); i++){
        InterfaceInfo if_info = info->interface_info(i);
        
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

        database_->QueueSqlStatement(if_insert.get_statement());
    }
}

void LogProtoHandler::ProcessLifecycleEvent(google::protobuf::MessageLite* message){
    re_common::LifecycleEvent* event = (re_common::LifecycleEvent*)message;
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

void LogProtoHandler::ProcessMessageEvent(google::protobuf::MessageLite* message){
    re_common::MessageEvent* event = (re_common::MessageEvent*)message;
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

void LogProtoHandler::ProcessClientEvent(std::string client_endpoint){
    /*auto ins = table_map_[LOGAN_CLIENT_TABLE]->get_insert_statement();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    ins.BindDouble(LOGAN_TIMEOFDAY, time.count()/1000.0);
    ins.BindString("endpoint", client_endpoint);
    database_->QueueSqlStatement(ins.get_statement());
    */
}

void LogProtoHandler::ProcessUserEvent(google::protobuf::MessageLite* message){

    re_common::UserEvent* event = (re_common::UserEvent*)message;
    auto ins = table_map_[LOGAN_USER_EVENT_TABLE]->get_insert_statement();
    ins.BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
    ins.BindString(LOGAN_HOSTNAME, event->info().hostname());
    ins.BindString(LOGAN_COMPONENT_NAME, event->component().name());
    ins.BindString(LOGAN_COMPONENT_ID, event->component().id());
    ins.BindString("message", event->message());
    ins.BindString("type", re_common::UserEvent::Type_Name(event->type()));
    database_->QueueSqlStatement(ins.get_statement());
}

void LogProtoHandler::ProcessWorkloadEvent(google::protobuf::MessageLite* message){
    re_common::WorkloadEvent* event = (re_common::WorkloadEvent*)message;
    auto ins = table_map_[LOGAN_WORKLOAD_EVENT_TABLE]->get_insert_statement();
    ins.BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
    ins.BindString(LOGAN_HOSTNAME, event->info().hostname());
    ins.BindString("event_type", re_common::WorkloadEvent::Type_Name(event->event_type()));
    ins.BindString("workload_implementation_id", event->id());
    ins.BindString("function_name", event->name());
    ins.BindString("function_library", event->type());
    
    ins.BindString(LOGAN_COMPONENT_NAME, event->component().name());
    ins.BindString(LOGAN_COMPONENT_ID, event->component().id());
    ins.BindString("type", event->component().type());
    ins.BindString("description", event->description());
}
