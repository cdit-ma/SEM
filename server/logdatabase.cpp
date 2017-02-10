#include "logdatabase.h"
#include "tableinsert.h"
#include <iostream>

#define LOGAN_DECIMAL "DECIMAL"
#define LOGAN_VARCHAR "VARCHAR"
#define LOGAN_INT "INTEGER"

#define LOGAN_TIMEOFDAY "timeofday"
#define LOGAN_HOSTNAME "hostname"
#define LOGAN_COMPONENT_NAME "component_name"
#define LOGAN_COMPONENT_ID "component_id"
#define LOGAN_PORT_NAME "port_name"
#define LOGAN_PORT_ID "port_id"
#define LOGAN_EVENT "event"
#define LOGAN_MESSAGE_ID "id"
#define LOGAN_NAME "name"

LogDatabase::LogDatabase(std::string databaseFilepath):SQLiteDatabase(databaseFilepath){
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

    for(auto pair : table_map_){
        auto t = pair.second;
        QueueSqlStatement(t->get_table_construct_statement());
    }

    Flush();
}

void LogDatabase::SystemStatusTable(){
    std::string name = "Hardware_SystemStatus";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("cpu_utilization", LOGAN_DECIMAL);
    t->AddColumn("phys_mem_utilization", LOGAN_DECIMAL);
    table_map_[name] = t;
}

void LogDatabase::SystemInfoTable(){
    std::string name = "Hardware_SystemInfo";
    Table* t = new Table(database_, name);
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
    table_map_[name] = t;
}

void LogDatabase::CpuTable(){
    std::string name = "Hardware_SystemCPUCoreStatus";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("core_id", LOGAN_INT);
    t->AddColumn("core_utilization", LOGAN_DECIMAL);
    table_map_[name] = t;
}

void LogDatabase::FileSystemTable(){
    std::string name = "Hardware_FileSystemStatus";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("utilization", LOGAN_DECIMAL);
    table_map_[name] = t;
}

void LogDatabase::FileSystemInfoTable(){
    std::string name = "Hardware_FileSystemInfo";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("type", LOGAN_VARCHAR);
    t->AddColumn("size", LOGAN_INT);
    table_map_[name] = t;
}

void LogDatabase::InterfaceTable(){
    std::string name = "Hardware_InterfaceStatus";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("rx_packets", LOGAN_INT);
    t->AddColumn("rx_bytes", LOGAN_INT);
    t->AddColumn("tx_packets", LOGAN_INT);
    t->AddColumn("tx_bytes", LOGAN_INT);
    table_map_[name] = t;
}

void LogDatabase::InterfaceInfoTable(){
    std::string name = "Hardware_InterfaceInfo";
    Table* t = new Table(database_, name);
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
    table_map_[name] = t;
}

void LogDatabase::ProcessTable(){
    std::string name = "Hardware_ProcessStatus";
    Table* t = new Table(database_, name);
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
    
    table_map_[name] = t;
}

void LogDatabase::ProcessInfoTable(){
    std::string name = "Hardware_ProcessInfo";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_MESSAGE_ID, LOGAN_INT);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn("pid", LOGAN_INT);
    t->AddColumn(LOGAN_NAME, LOGAN_VARCHAR);
    t->AddColumn("args", LOGAN_VARCHAR);
    t->AddColumn("start_time", LOGAN_INT);
    
    table_map_[name] = t;
}

void LogDatabase::PortEventTable(){
    std::string name = "Lifecycle_PortEvent";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_PORT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_EVENT, LOGAN_VARCHAR);
    
    table_map_[name] = t;
}

void LogDatabase::ComponentEventTable(){
    std::string name = "Lifecycle_ComponentEvent";
    Table* t = new Table(database_, name);
    t->AddColumn(LOGAN_TIMEOFDAY, LOGAN_DECIMAL);
    t->AddColumn(LOGAN_HOSTNAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_NAME, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_COMPONENT_ID, LOGAN_VARCHAR);
    t->AddColumn(LOGAN_EVENT, LOGAN_VARCHAR);
    
    table_map_[name] = t;
}

std::string LogDatabase::get_port_event_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS Events_Port ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "timeofday DECIMAL,"                            //1
            "hostname VARCHAR,"                             //2
            "component_name VARCHAR,"                       //3
            "component_id VARCHAR,"                         //4
            "port_name VARCHAR,"                            //5
            "port_id VARCHAR,"                              //6
            "port_type VARCHAR"                             //7
            ");";
}
std::string LogDatabase::get_port_event_insert_query() const{
    return  "INSERT INTO Events_Port (" 
            "timeofday,"                                    //1
            "hostname,"                                     //2
            "component_name,"                               //3
            "component_id,"                                 //4
            "port_name,"                                    //5
            "port_id,"                                      //6
            "port_type"                                     //7
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);";
}



int LogDatabase::bind_string(sqlite3_stmt* stmnt, int pos, std::string str){
    return sqlite3_bind_text(stmnt, pos, str.c_str(), str.size(), SQLITE_TRANSIENT);
}

void LogDatabase::ProcessSystemStatus(SystemStatus* status){
    auto stmt = table_map_["Hardware_SystemStatus"]->get_insert_statement();

    std::string hostname = status->hostname().c_str();
    int message_id = status->message_id();
    double timestamp = status->timestamp();
    
    stmt->BindString(LOGAN_HOSTNAME, hostname);
    stmt->BindInt(LOGAN_MESSAGE_ID, message_id);
    stmt->BindDouble(LOGAN_TIMEOFDAY, timestamp);
    stmt->BindDouble("cpu_utilization", status->cpu_utilization());
    stmt->BindDouble("phys_mem_utilization", status->phys_mem_utilization());
    QueueSqlStatement(stmt->get_statement());


    if(status->has_info() != 0){
        auto infostmt = table_map_["Hardware_SystemInfo"]->get_insert_statement();
        infostmt->BindString(LOGAN_HOSTNAME, hostname);
        infostmt->BindInt(LOGAN_MESSAGE_ID, message_id);
        infostmt->BindDouble(LOGAN_TIMEOFDAY, timestamp);
        
        SystemStatus::SystemInfo info = status->info();
        infostmt->BindString("os_name", info.os_name());
        infostmt->BindString("os_arch", info.os_arch());
        infostmt->BindString("os_description", info.os_description());
        infostmt->BindString("os_version", info.os_version());
        infostmt->BindString("os_vendor", info.os_vendor());
        infostmt->BindString("os_vendor_name", info.os_vendor_name());
        infostmt->BindString("cpu_model", info.cpu_model());
        infostmt->BindString("cpu_vendor", info.cpu_vendor());
        infostmt->BindInt("cpu_frequency", info.cpu_frequency());
        infostmt->BindInt("physical_memory", info.physical_memory());
        QueueSqlStatement(infostmt->get_statement());
    }


    for(int i = 0; i < status->cpu_core_utilization_size(); i++){
        auto cpustmt = table_map_["Hardware_SystemCPUCoreStatus"]->get_insert_statement();        
        cpustmt->BindString(LOGAN_HOSTNAME, hostname);
        cpustmt->BindInt(LOGAN_MESSAGE_ID, message_id);
        cpustmt->BindDouble(LOGAN_TIMEOFDAY, timestamp);
        cpustmt->BindInt("core_id", i);
        cpustmt->BindDouble("core_utilization", status->cpu_core_utilization(i));
        QueueSqlStatement(cpustmt->get_statement());
    }

    for(int i = 0; i < status->processes_size(); i++){
        auto procstmt = table_map_["Hardware_ProcessStatus"]->get_insert_statement();
        ProcessStatus proc = status->processes(i);

        if(proc.has_info()){
            auto procinfo = table_map_["Hardware_ProcessInfo"]->get_insert_statement();
            procinfo->BindString(LOGAN_HOSTNAME, hostname);
            procinfo->BindInt(LOGAN_MESSAGE_ID, message_id);
            procinfo->BindDouble(LOGAN_TIMEOFDAY, timestamp);
            procinfo->BindInt("pid", proc.pid());
            procinfo->BindString(LOGAN_NAME, proc.info().name());
            procinfo->BindString("args", proc.info().args());
            procinfo->BindInt("start_time", proc.info().start_time());
            QueueSqlStatement(procinfo->get_statement());
        }
        procstmt->BindString(LOGAN_HOSTNAME, hostname);
        procstmt->BindInt(LOGAN_MESSAGE_ID, message_id);
        procstmt->BindDouble(LOGAN_TIMEOFDAY, timestamp);
        procstmt->BindInt("pid", proc.pid());
        procstmt->BindInt("core_id", proc.cpu_core_id());
        procstmt->BindDouble("cpu_utilization", proc.cpu_utilization());
        procstmt->BindDouble("phys_mem_utilization", proc.phys_mem_utilization());
        procstmt->BindInt("thread_count", proc.thread_count());
        procstmt->BindInt("disk_read", proc.disk_read());
        procstmt->BindInt("disk_written", proc.disk_written());
        procstmt->BindInt("disk_total", proc.disk_total());
        procstmt->BindString("state", ProcessStatus::State_Name(proc.state()));
        QueueSqlStatement(procstmt->get_statement());
    }

    for(int i = 0; i < status->interfaces_size(); i++){
        auto ifstatement = table_map_["Hardware_InterfaceStatus"]->get_insert_statement();        
        InterfaceStatus ifstat = status->interfaces(i);

        if(ifstat.has_info()){
            auto ifinfo = table_map_["Hardware_InterfaceInfo"]->get_insert_statement();        
            
            ifinfo->BindString(LOGAN_HOSTNAME, hostname);
            ifinfo->BindInt(LOGAN_MESSAGE_ID, message_id);
            ifinfo->BindDouble(LOGAN_TIMEOFDAY, timestamp);

            ifinfo->BindString(LOGAN_NAME, ifstat.name());
            ifinfo->BindString("type", ifstat.info().type());
            ifinfo->BindString("description", ifstat.info().description());
            ifinfo->BindString("ipv4_addr", ifstat.info().ipv4_addr());
            ifinfo->BindString("ipv6_addr", ifstat.info().ipv6_addr());
            ifinfo->BindString("mac_addr", ifstat.info().mac_addr());
            ifinfo->BindInt("speed", ifstat.info().speed());

            QueueSqlStatement(ifinfo->get_statement());
        }

        ifstatement->BindString(LOGAN_HOSTNAME, hostname);
        ifstatement->BindInt(LOGAN_MESSAGE_ID, message_id);
        ifstatement->BindDouble(LOGAN_TIMEOFDAY, timestamp);
        ifstatement->BindString(LOGAN_NAME, ifstat.name());
        ifstatement->BindInt("rx_packets", ifstat.rx_packets());
        ifstatement->BindInt("rx_bytes", ifstat.rx_bytes());
        ifstatement->BindInt("tx_packets", ifstat.tx_packets());
        ifstatement->BindInt("tx_bytes", ifstat.tx_bytes());
        QueueSqlStatement(ifstatement->get_statement());
    }

    for(int i = 0; i < status->file_systems_size(); i++){
        auto fsstatement = table_map_["Hardware_FileSystemStatus"]->get_insert_statement();                
        FileSystemStatus fss = status->file_systems(i);

        if(fss.has_info()){
            auto fsinfo = table_map_["Hardware_FileSystemInfo"]->get_insert_statement();

            fsinfo->BindString(LOGAN_HOSTNAME, hostname);
            fsinfo->BindInt(LOGAN_MESSAGE_ID, message_id);
            fsinfo->BindDouble(LOGAN_TIMEOFDAY, timestamp);                        
            fsinfo->BindString(LOGAN_NAME, fss.name());
            fsinfo->BindString("type", FileSystemStatus::FileSystemInfo::Type_Name(fss.info().type()));
            fsinfo->BindInt("size", fss.info().size());
            QueueSqlStatement(fsinfo->get_statement());
        }

        fsstatement->BindString(LOGAN_HOSTNAME, hostname);
        fsstatement->BindInt(LOGAN_MESSAGE_ID, message_id);
        fsstatement->BindDouble(LOGAN_TIMEOFDAY, timestamp);
        fsstatement->BindString(LOGAN_NAME, fss.name());
        fsstatement->BindDouble("utilization", fss.utilization());
        QueueSqlStatement(fsstatement->get_statement());
    }

}

void LogDatabase::ProcessLifecycleEvent(re_common::LifecycleEvent* event){
    if(event->has_port() && event->has_component()){
        //Process port event
        std::cout << "process port called" << std::endl;
        //Insert test Statements
        auto ins = table_map_["Lifecycle_PortEvent"]->get_insert_statement();
        ins->BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
        ins->BindString(LOGAN_HOSTNAME, event->info().hostname());
        ins->BindString(LOGAN_COMPONENT_NAME, event->component().name());
        ins->BindString(LOGAN_COMPONENT_ID, event->component().id());
        ins->BindString(LOGAN_PORT_NAME, event->port().name());
        ins->BindString(LOGAN_PORT_ID, event->port().id());
        ins->BindString(LOGAN_EVENT, re_common::LifecycleEvent::Type_Name(event->type()));

        QueueSqlStatement(ins->get_statement());
    }

    else if(event->has_component()){
        std::cout << "process event called" << std::endl;
            auto ins = table_map_["Lifecycle_ComponentEvent"]->get_insert_statement();
            ins->BindDouble(LOGAN_TIMEOFDAY, event->info().timestamp());
            ins->BindString(LOGAN_HOSTNAME, event->info().hostname());
            ins->BindString(LOGAN_COMPONENT_NAME, event->component().name());
            ins->BindString(LOGAN_COMPONENT_ID, event->component().id());
            QueueSqlStatement(ins->get_statement());
    }
}

std::string LogDatabase::process_state_to_string(const ProcessStatus::State state) const{
    switch(state){
        case ProcessStatus::PROC_ERROR:
            return "ERROR";
        case ProcessStatus::PROC_RUNNING:
            return "RUNNING";
        case ProcessStatus::PROC_SLEEPING:
            return "SLEEPING";
        case ProcessStatus::PROC_DISK_SLEEP:
            return "DISK SLEEP";
        case ProcessStatus::PROC_STOPPED:
            return "STOPPED";
        case ProcessStatus::PROC_ZOMBIE:
            return "ZOMBIE";
        case ProcessStatus::PROC_DEAD:
            return "DEAD";
        default:
            return "ERROR";
    }
}

std::string LogDatabase::fs_type_to_string(const FileSystemStatus::FileSystemInfo::Type type) const{
    switch(type){
        case FileSystemStatus::FileSystemInfo::FS_UNKNOWN:
            return "UNKNOWN";
        case FileSystemStatus::FileSystemInfo::FS_LOCAL_DISK:
            return "LOCAL_DISK";
        case FileSystemStatus::FileSystemInfo::FS_NETWORK:
            return "NETWORK";
        case FileSystemStatus::FileSystemInfo::FS_RAM_DISK:
            return "RAM_DISK";
        case FileSystemStatus::FileSystemInfo::FS_CDROM:
            return "CDROM";
        case FileSystemStatus::FileSystemInfo::FS_SWAP:
            return "SWAP";
        default:
            return "";
    }
}
