#include "logdatabase.h"
#include <iostream>

LogDatabase::LogDatabase(std::string databaseFilepath):SQLiteDatabase(databaseFilepath){
    //Construct all of our tables
    QueueSqlStatement(GetSqlStatement(get_system_status_table_string()));
    QueueSqlStatement(GetSqlStatement(get_system_info_table_string()));
    QueueSqlStatement(GetSqlStatement(get_cpu_table_string()));
    QueueSqlStatement(GetSqlStatement(get_fs_table_string()));
    QueueSqlStatement(GetSqlStatement(get_fs_info_table_string()));
    QueueSqlStatement(GetSqlStatement(get_interface_table_string()));
    QueueSqlStatement(GetSqlStatement(get_interface_info_table_string()));
    QueueSqlStatement(GetSqlStatement(get_process_table_string()));
    QueueSqlStatement(GetSqlStatement(get_process_info_table_string()));
    QueueSqlStatement(GetSqlStatement(get_component_lifecycle_table_string()));
    QueueSqlStatement(GetSqlStatement(get_port_lifecycle_table_string()));

    QueueSqlStatement(GetSqlStatement(get_port_event_table_string()));
    //Force the tables to be constructed
    Flush();
}

std::string LogDatabase::get_system_status_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS SystemStatus (" 
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "cpu_utilization DECIMAL,"                      //4
            "phys_mem_utilization DECIMAL"                  //5
            ");";
}

std::string LogDatabase::get_system_status_insert_query() const{
    return  "INSERT INTO SystemStatus ("
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "cpu_utilization,"                              //4
            "phys_mem_utilization"                          //5
            ") VALUES (?1, ?2, ?3, ?4, ?5);";
}

std::string LogDatabase::get_system_info_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS SystemInfo ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "os_name VARCHAR,"                              //4
            "os_arch VARCHAR,"                              //5
            "os_description VARCHAR,"                       //6
            "os_version VARCHAR,"                           //7
            "os_vendor VARCHAR,"                            //8
            "os_vendor_name VARCHAR,"                       //9
            "cpu_model VARCHAR,"                            //10
            "cpu_vendor VARCHAR,"                           //11
            "cpu_frequency INTEGER,"                        //12
            "physical_memory INTEGER"                       //13
            ");";
}

std::string LogDatabase::get_system_info_insert_query() const{
    return  "INSERT INTO SystemInfo (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "os_name,"                                      //4
            "os_arch,"                                      //5
            "os_description,"                               //6
            "os_version,"                                   //7
            "os_vendor,"                                    //8
            "os_vendor_name,"                               //9
            "cpu_model,"                                    //10
            "cpu_vendor,"                                   //11
            "cpu_frequency,"                                //12
            "physical_memory"                               //13
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13);";
}

std::string LogDatabase::get_cpu_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS SystemCPUCoreStatus ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "core_id INTEGER,"                              //4
            "core_utilization DECIMAL"                      //5
            ");";
}

std::string LogDatabase::get_cpu_insert_query() const{
    return  "INSERT INTO SystemCPUCoreStatus (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "core_id,"                                      //4
            "core_utilization"                              //5
            ") VALUES (?1, ?2, ?3, ?4, ?5);";
}

std::string LogDatabase::get_fs_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS FileSystemStatus ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "name VARCHAR,"                                 //4
            "utilization DECIMAL"                           //5
            ");";
}

std::string LogDatabase::get_fs_insert_query() const{
    return  "INSERT INTO FileSystemStatus (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "name,"                                         //4
            "utilization"                                   //5
            ") VALUES (?1, ?2, ?3, ?4, ?5);";
}

std::string LogDatabase::get_fs_info_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS FileSystemInfo ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "name VARCHAR,"                                 //4
            "type VARCHAR,"                                 //5
            "size INTEGER"                                  //6
            ");";
}
std::string LogDatabase::get_fs_info_insert_query() const{
    return  "INSERT INTO FileSystemInfo (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "name,"                                         //4
            "type,"                                         //5
            "size"                                          //6
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6);";
}

std::string LogDatabase::get_interface_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS InterfaceStatus ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "name VARCHAR,"                                 //4
            "rx_packets INTEGER,"                           //5
            "rx_bytes INTEGER,"                             //6
            "tx_packets INTEGER,"                           //7
            "tx_bytes INTEGER"                              //8
            ");";
}

std::string LogDatabase::get_interface_insert_query() const{
    return  "INSERT INTO InterfaceStatus (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "name,"                                         //4
            "rx_packets,"                                   //5
            "rx_bytes,"                                     //6
            "tx_packets,"                                   //7
            "tx_bytes"                                      //8
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);";
}

std::string LogDatabase::get_interface_info_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS InterfaceInfo ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"        
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "name VARCHAR,"                                 //4
            "type VARCHAR,"                                 //5
            "description VARCHAR,"                          //6
            "ipv4_addr VARCHAR,"                            //7
            "ipv6_addr VARCHAR,"                            //8
            "mac_addr VARCHAR,"                             //9
            "speed INTEGER"                                 //10
            ");";
}

std::string LogDatabase::get_interface_info_insert_query() const{
    return  "INSERT INTO InterfaceInfo (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "name,"                                         //4
            "type,"                                         //5
            "description,"                                  //6
            "ipv4_addr,"                                    //7
            "ipv6_addr,"                                    //8
            "mac_addr,"                                     //9
            "speed"                                        //10
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10);";
}

std::string LogDatabase::get_process_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS ProcessStatus ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"    
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "pid INTEGER,"                                  //4
            "core_id INTEGER,"                              //5
            "cpu_utilization DECIMAL,"                      //6
            "phys_mem_utilization DECIMAL,"                 //7
            "thread_count INTEGER,"                         //8
            "disk_read INTEGER,"                            //9
            "disk_written INTEGER,"                         //10
            "disk_total INTEGER,"                           //11
            "state VARCHAR"                                 //12
            ");";
}

std::string LogDatabase::get_process_insert_query() const{
    return  "INSERT INTO ProcessStatus (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "pid,"                                          //4
            "core_id,"                                      //5
            "cpu_utilization,"                              //6
            "phys_mem_utilization,"                         //7
            "thread_count,"                                 //8
            "disk_read,"                                    //9
            "disk_written,"                                 //10
            "disk_total,"                                   //11
            "state"                                         //12
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12);";
}

std::string LogDatabase::get_process_info_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS ProcessInfo ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "hostname VARCHAR,"                             //1
            "message_id INTEGER,"                           //2
            "timeofday DECIMAL,"                            //3
            "pid INTEGER,"                                  //4
            "name VARCHAR,"                                 //5
            "args VARCHAR,"                                 //6
            "start_time INTEGER"                            //7
            ");";
}

std::string LogDatabase::get_process_info_insert_query() const{
    return  "INSERT INTO ProcessInfo (" 
            "hostname,"                                     //1
            "message_id,"                                   //2
            "timeofday,"                                    //3
            "pid,"                                          //4
            "name,"                                         //5
            "args,"                                         //6 
            "start_time"                                    //7
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);";
}

std::string LogDatabase::get_port_lifecycle_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS Lifecycle_Port ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "timeofday DECIMAL,"                            //1
            "hostname VARCHAR,"                             //2
            "component_name VARCHAR,"                       //3
            "component_id VARCHAR,"                         //4
            "port_name VARCHAR,"                            //5
            "port_id VARCHAR,"                              //6
            "event VARCHAR"                                 //7
            ");";
}

std::string LogDatabase::get_port_lifecycle_insert_query() const{
    return  "INSERT INTO Lifecycle_Port (" 
            "timeofday,"                                    //1
            "hostname,"                                     //2
            "component_name,"                               //3
            "component_id,"                                 //4
            "port_name,"                                    //5
            "port_id,"                                      //6
            "event"                                         //7
            ") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);";
}
std::string LogDatabase::get_component_lifecycle_table_string() const{
    return  "CREATE TABLE IF NOT EXISTS Lifecycle_Component ("
            "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
            "timeofday DECIMAL,"                            //1
            "hostname VARCHAR,"                             //2
            "component_name VARCHAR,"                       //3
            "component_id VARCHAR,"                         //4
            "event VARCHAR"                                 //5
            ");";
}

std::string LogDatabase::get_component_lifecycle_insert_query() const{
    return  "INSERT INTO Lifecycle_Component (" 
            "timeofday,"                                    //1
            "hostname,"                                     //2
            "component_name,"                               //3
            "component_id,"                                 //4
            "event"                                         //5
            ") VALUES (?1, ?2, ?3, ?4, ?5);";
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
    sqlite3_stmt *stmt = GetSqlStatement(get_system_status_insert_query());

    std::string hostname = status->hostname().c_str();
    int message_id = status->message_id();
    double timestamp = status->timestamp();
    

    bind_string(stmt, 1, hostname);
    sqlite3_bind_int(stmt, 2, message_id);
    sqlite3_bind_double(stmt, 3, timestamp);
    sqlite3_bind_double(stmt, 4, status->cpu_utilization());
    sqlite3_bind_double(stmt, 5, status->phys_mem_utilization());
    QueueSqlStatement(stmt);

    if(status->has_info() != 0){
        sqlite3_stmt *infostmt = GetSqlStatement(get_system_info_insert_query());
        bind_string(infostmt, 1, hostname);
        sqlite3_bind_int(infostmt, 2, message_id);
        sqlite3_bind_double(infostmt, 3, timestamp);
        
        
        SystemStatus::SystemInfo info = status->info();
        
        
        bind_string(infostmt, 4, info.os_name());
        bind_string(infostmt, 5, info.os_arch());
        bind_string(infostmt, 6, info.os_description());
        bind_string(infostmt, 7, info.os_version());
        bind_string(infostmt, 8, info.os_vendor());
        bind_string(infostmt, 9, info.os_vendor_name());
        bind_string(infostmt, 10, info.cpu_model());
        bind_string(infostmt, 11, info.cpu_vendor());
        sqlite3_bind_int(infostmt, 12, info.cpu_frequency());
        sqlite3_bind_int(infostmt, 13, info.physical_memory());
        QueueSqlStatement(infostmt);
    }


    for(int i = 0; i < status->cpu_core_utilization_size(); i++){
        sqlite3_stmt *cpustmt = GetSqlStatement(get_cpu_insert_query());
        bind_string(cpustmt, 1, hostname);
        sqlite3_bind_int(cpustmt, 2, message_id);
        sqlite3_bind_double(cpustmt, 3, timestamp);
        sqlite3_bind_int(cpustmt, 4, i);
        sqlite3_bind_double(cpustmt, 5, status->cpu_core_utilization(i));
        QueueSqlStatement(cpustmt);
    }

    for(int i = 0; i < status->processes_size(); i++){
        sqlite3_stmt *procstmt = GetSqlStatement(get_process_insert_query());
        ProcessStatus proc = status->processes(i);

        if(proc.has_info()){
            sqlite3_stmt *procinfo = GetSqlStatement(get_process_info_insert_query());
            bind_string(procinfo, 1, hostname);
            sqlite3_bind_int(procinfo, 2, message_id);
            sqlite3_bind_double(procinfo, 3, timestamp);
            sqlite3_bind_int(procinfo, 4, proc.pid());
            bind_string(procinfo, 5, proc.info().name());
            bind_string(procinfo, 6, proc.info().args());
            sqlite3_bind_int(procinfo, 7, proc.info().start_time());    
            QueueSqlStatement(procinfo);
        }
        bind_string(procstmt, 1, hostname);
        sqlite3_bind_int(procstmt, 2, message_id);
        sqlite3_bind_double(procstmt, 3, timestamp);
        sqlite3_bind_int(procstmt, 4, proc.pid());
        sqlite3_bind_int(procstmt, 5, proc.cpu_core_id());
        sqlite3_bind_double(procstmt, 6, proc.cpu_utilization());
        sqlite3_bind_double(procstmt, 7, proc.phys_mem_utilization());
        sqlite3_bind_int(procstmt, 8, proc.thread_count());
        
        sqlite3_bind_int(procstmt, 9, proc.disk_read());
        
        sqlite3_bind_int(procstmt, 10, proc.disk_written());
        
        sqlite3_bind_int(procstmt, 11, proc.disk_total());
        
        std::string procstate = process_state_to_string(proc.state());
        bind_string(procstmt, 12, procstate);
        QueueSqlStatement(procstmt);
    }

    for(int i = 0; i < status->interfaces_size(); i++){
        sqlite3_stmt *ifstatement = GetSqlStatement(get_interface_insert_query());
        InterfaceStatus ifstat = status->interfaces(i);

        if(ifstat.has_info()){
            sqlite3_stmt *ifinfo = GetSqlStatement(get_interface_info_insert_query());
            
            bind_string(ifinfo, 1, hostname);
            sqlite3_bind_int(ifinfo, 2, message_id);
            sqlite3_bind_double(ifinfo, 3, timestamp);
            bind_string(ifinfo, 4, ifstat.name());
            bind_string(ifinfo, 5, ifstat.info().type());
            bind_string(ifinfo, 6, ifstat.info().description());
            bind_string(ifinfo, 7, ifstat.info().ipv4_addr());
            bind_string(ifinfo, 8, ifstat.info().ipv6_addr());
            bind_string(ifinfo, 9, ifstat.info().mac_addr());
            sqlite3_bind_int(ifinfo, 10, ifstat.info().speed());
            QueueSqlStatement(ifinfo);
        }
        bind_string(ifstatement, 1, hostname);
        sqlite3_bind_int(ifstatement, 2, message_id);
        sqlite3_bind_double(ifstatement, 3, timestamp);
        bind_string(ifstatement, 4, ifstat.name());
        sqlite3_bind_int(ifstatement, 5, ifstat.rx_packets());
        sqlite3_bind_int(ifstatement, 6, ifstat.rx_bytes());
        sqlite3_bind_int(ifstatement, 7, ifstat.tx_packets());
        sqlite3_bind_int(ifstatement, 8, ifstat.tx_bytes());
        QueueSqlStatement(ifstatement);
    }

    for(int i = 0; i < status->file_systems_size(); i++){
        sqlite3_stmt *fsstatement = GetSqlStatement(get_fs_insert_query());
        FileSystemStatus fss = status->file_systems(i);

        if(fss.has_info()){
            sqlite3_stmt *fsinfo = GetSqlStatement(get_fs_info_insert_query());
            bind_string(fsinfo, 1, hostname);
            sqlite3_bind_int(fsinfo, 2, message_id);
            sqlite3_bind_double(fsinfo, 3, timestamp);
            bind_string(fsinfo, 4, fss.name());
            std::string type = fs_type_to_string(fss.info().type());
            bind_string(fsinfo, 5, type);
            sqlite3_bind_int(fsinfo, 6, fss.info().size());
            QueueSqlStatement(fsinfo);
        }
        bind_string(fsstatement, 1, hostname);
        sqlite3_bind_int(fsstatement, 2, message_id);
        sqlite3_bind_double(fsstatement, 3, timestamp);
        bind_string(fsstatement, 4, fss.name());
        sqlite3_bind_double(fsstatement, 5, fss.utilization());
        QueueSqlStatement(fsstatement);
    }

}

void LogDatabase::ProcessLifecycleEvent(re_common::LifecycleEvent* event){
    if(event->has_port() && event->has_component()){
        //Process port event
        std::cout << "process port called" << std::endl;        
        sqlite3_stmt* port_statement = GetSqlStatement(get_port_lifecycle_insert_query());
        sqlite3_bind_double(port_statement, 1, event->info().timestamp());
        bind_string(port_statement, 2, event->info().hostname());
        bind_string(port_statement, 3, event->component().name());
        bind_string(port_statement, 4, event->component().id());
        bind_string(port_statement, 5, event->port().name());
        bind_string(port_statement, 6, event->port().id());
        bind_string(port_statement, 7, re_common::LifecycleEvent::Type_Name(event->type()));
        QueueSqlStatement(port_statement);
    }

    else if(event->has_component()){
        std::cout << "process event called" << std::endl;
        //Process component event
        sqlite3_stmt* component_statement = GetSqlStatement(get_component_lifecycle_insert_query());
        sqlite3_bind_double(component_statement, 1, event->info().timestamp());
        bind_string(component_statement, 2, event->info().hostname());
        bind_string(component_statement, 3, event->component().name());
        bind_string(component_statement, 4, event->component().id());
        bind_string(component_statement, 5, re_common::LifecycleEvent::Type_Name(event->type()));
        QueueSqlStatement(component_statement);
    }
}

void LogDatabase::ProcessMessageEvent(re_common::MessageEvent* event){
    if(event->has_port() && event->has_component()){
        //Process port event
        std::cout << "process ProcessMessageEvent" << std::endl;        
        sqlite3_stmt* port_statement = GetSqlStatement(get_port_event_insert_query());
        sqlite3_bind_double(port_statement, 1, event->info().timestamp());
        bind_string(port_statement, 2, event->info().hostname());
        bind_string(port_statement, 3, event->component().name());
        bind_string(port_statement, 4, event->component().id());
        bind_string(port_statement, 5, event->port().name());
        bind_string(port_statement, 6, event->port().id());
        bind_string(port_statement, 7, re_common::Port::PortKind_Name(event->port().kind()));
        QueueSqlStatement(port_statement);
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
/*
std::string LogDatabase::action_type_to_string(const ModelEvent::ActionType type) const{
    switch(type){
        case ModelEvent::ActionType::STARTED:
            return "STARTED";
        case ModelEvent::ActionType::ACTIVATED:
            return "ACTIVATED";
        case ModelEvent::ActionType::PASSIVATED:
            return "PASSIVATED";
        case ModelEvent::ActionType::TERMINATED:
            return "TERMINATED";
        default:
            return "";
    }
}

std::string LogDatabase::port_type_to_string(const ModelEvent::PortEvent::PortType type) const{
    switch(type){
        case  ModelEvent::PortEvent::PortType::TX:
            return "TX";
        case  ModelEvent::PortEvent::PortType::RX:
            return "RX";
        case  ModelEvent::PortEvent::PortType::PE:
            return "PE";
        default:
            return "";
    }
}

*/