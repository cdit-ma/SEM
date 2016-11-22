#include "logdatabase.h"
#include <iostream>
LogDatabase::LogDatabase(std::string databaseFilepath):SQLiteDatabase(databaseFilepath){
    queue_sql_statement(get_sql_statement(get_system_status_table_string()));
    queue_sql_statement(get_sql_statement(get_system_info_table_string()));
    queue_sql_statement(get_sql_statement(get_cpu_table_string()));
    queue_sql_statement(get_sql_statement(get_fs_table_string()));
    queue_sql_statement(get_sql_statement(get_fs_info_table_string()));
    queue_sql_statement(get_sql_statement(get_interface_table_string()));
    queue_sql_statement(get_sql_statement(get_interface_info_table_string()));
    queue_sql_statement(get_sql_statement(get_process_table_string()));
    queue_sql_statement(get_sql_statement(get_process_info_table_string()));
    
    flush();
}

std::string LogDatabase::get_system_status_table_string() const{
    return "CREATE TABLE IF NOT EXISTS SystemStatus (" 
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "cpu_utilization DECIMAL,"
        "phys_mem_utilization DECIMAL"
        ");";
}

std::string LogDatabase::get_system_status_insert_query() const{
    return "INSERT INTO SystemStatus" 
    "(hostname, message_id, timeofday, cpu_utilization, phys_mem_utilization)" 
    "VALUES (?1, ?2, ?3, ?4, ?5);";
}

std::string LogDatabase::get_system_info_table_string() const{
    return "CREATE TABLE IF NOT EXISTS SystemInfo ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "os_name VARCHAR,"
        "os_arch VARCHAR,"
        "os_description VARCHAR,"
        "os_version VARCHAR,"
        "os_vendor VARCHAR,"
        "os_vendor_name VARCHAR,"
        "cpu_model VARCHAR,"
        "cpu_vendor VARCHAR,"
        "cpu_frequency INTEGER,"
        "physical_memory INTEGER"
        ");";
}

std::string LogDatabase::get_system_info_insert_query() const{
    return "INSERT INTO SystemInfo" 
    "(hostname, message_id, timeofday, os_name, os_arch, os_description, os_version, os_vendor, os_vendor_name, "
    "cpu_model, cpu_vendor, cpu_frequency, physical_memory) " 
    "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13);";
}

std::string LogDatabase::get_cpu_table_string() const{
    return "CREATE TABLE IF NOT EXISTS SystemCPUCoreStatus ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "core_id INTEGER,"
        "core_utilization DECIMAL"
        ");";
}

std::string LogDatabase::get_cpu_insert_query() const{
    return "INSERT INTO SystemCPUCoreStatus" 
    "(hostname, message_id, timeofday, core_id, core_utilization) " 
    "VALUES (?1, ?2, ?3, ?4, ?5);";
}

std::string LogDatabase::get_fs_table_string() const{
    return "CREATE TABLE IF NOT EXISTS FileSystemStatus ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "name VARCHAR,"
        "utilization DECIMAL"
        ");";
}

std::string LogDatabase::get_fs_insert_query() const{
    return "INSERT INTO FileSystemStatus" 
    "(hostname, message_id, timeofday, name, utilization) " 
    "VALUES (?1, ?2, ?3, ?4, ?5);";
}

std::string LogDatabase::get_fs_info_table_string() const{
    return "CREATE TABLE IF NOT EXISTS FileSystemInfo ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "name VARCHAR,"
        "type VARCHAR,"
        "size INTEGER"
        ");";
}
std::string LogDatabase::get_fs_info_insert_query() const{
    return "INSERT INTO FileSystemInfo" 
    "(hostname, message_id, timeofday, name, type, size) " 
    "VALUES (?1, ?2, ?3, ?4, ?5, ?6);";
}

std::string LogDatabase::get_interface_table_string() const{
    return "CREATE TABLE IF NOT EXISTS InterfaceStatus ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "name VARCHAR,"
        "rx_packets INTEGER,"
        "rx_bytes INTEGER,"
        "tx_packets INTEGER,"
        "tx_bytes INTEGER"
        ");";
}

std::string LogDatabase::get_interface_insert_query() const{
    return "INSERT INTO InterfaceStatus" 
    "(hostname, message_id, timeofday, name, rx_packets, rx_bytes, tx_packets, tx_bytes) " 
    "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);";
}

std::string LogDatabase::get_interface_info_table_string() const{
    return "CREATE TABLE IF NOT EXISTS InterfaceInfo ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "name VARCHAR,"
        "type VARCHAR,"
        "description VARCHAR,"
        "ipv4_addr VARCHAR,"
        "ipv6_addr VARCHAR,"
        "mac_addr VARCHAR,"
        "speed INTEGER"
        ");";
}

std::string LogDatabase::get_interface_info_insert_query() const{
    return "INSERT INTO InterfaceInfo" 
    "(hostname, message_id, timeofday, name, type, description, ipv4_addr, ipv6_addr, mac_addr, speed) " 
    "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10);";
}

std::string LogDatabase::get_process_table_string() const{
    return "CREATE TABLE IF NOT EXISTS ProcessStatus ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"    
        "hostname VARCHAR,"                         //1
        "message_id INTEGER,"                       //2
        "timeofday DECIMAL,"                       //3
        "pid INTEGER,"                              //4
        "core_id INTEGER,"                          //5
        "cpu_utilization DECIMAL,"                  //6
        "phys_mem_utilization DECIMAL,"             //7
        "thread_count INTEGER,"                     //8
        "disk_read INTEGER,"                        //9
        "disk_written INTEGER,"                     //10
        "disk_total INTEGER,"                       //11
        "state VARCHAR"                             //12
        ");";
}

std::string LogDatabase::get_process_insert_query() const{
    return "INSERT INTO ProcessStatus" 
    "(hostname, message_id, timeofday, pid, core_id, cpu_utilization, phys_mem_utilization, thread_count, "
    "disk_read, disk_written, disk_total, state) " 
    "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12);";
}

std::string LogDatabase::get_process_info_table_string() const{
    return "CREATE TABLE IF NOT EXISTS ProcessInfo ("
        "lid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hostname VARCHAR,"
        "message_id INTEGER,"
        "timeofday DECIMAL,"
        "pid INTEGER,"
        "name VARCHAR,"
        "args VARCHAR,"
        "start_time INTEGER"
        ");";
}

std::string LogDatabase::get_process_info_insert_query() const{
    return "INSERT INTO ProcessInfo" 
    "(hostname, message_id, timeofday, pid, name, args, start_time) " 
    "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);";
}


void LogDatabase::process_status(SystemStatus* status){
    sqlite3_stmt *stmt = get_sql_statement(get_system_status_insert_query());

    std::string hostname = status->hostname().c_str();
    int message_id = status->message_id();
    double timestamp = status->timestamp();
    

    sqlite3_bind_text(stmt, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, message_id);
    sqlite3_bind_double(stmt, 3, timestamp);
    sqlite3_bind_double(stmt, 4, status->cpu_utilization());
    sqlite3_bind_double(stmt, 5, status->phys_mem_utilization());
    queue_sql_statement(stmt);

    if(status->has_info() != 0){
        sqlite3_stmt *infostmt = get_sql_statement(get_system_info_insert_query());
        sqlite3_bind_text(infostmt, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
        sqlite3_bind_int(infostmt, 2, message_id);
        sqlite3_bind_double(infostmt, 3, timestamp);
        SystemStatus::SystemInfo info = status->info();
        sqlite3_bind_text(infostmt, 4, info.os_name().c_str(), info.os_name().size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(infostmt, 5, info.os_arch().c_str(), info.os_arch().size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(infostmt, 6, info.os_description().c_str(), info.os_description().size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(infostmt, 7, info.os_version().c_str(), info.os_version().size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(infostmt, 8, info.os_vendor().c_str(), info.os_vendor().size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(infostmt, 9, info.os_vendor_name().c_str(), info.os_vendor_name().size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(infostmt, 10, info.cpu_model().c_str(), info.cpu_model().size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(infostmt, 11, info.cpu_vendor().c_str(), info.cpu_vendor().size(), SQLITE_TRANSIENT);
        sqlite3_bind_int(infostmt, 12, info.cpu_frequency());
        sqlite3_bind_int(infostmt, 13, info.physical_memory());
        queue_sql_statement(infostmt);
    }


    for(int i = 0; i < status->cpu_core_utilization_size(); i++){
        sqlite3_stmt *cpustmt = get_sql_statement(get_cpu_insert_query());
        sqlite3_bind_text(cpustmt, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
        sqlite3_bind_int(cpustmt, 2, message_id);
        sqlite3_bind_double(cpustmt, 3, timestamp);
        sqlite3_bind_int(cpustmt, 4, i);
        sqlite3_bind_double(cpustmt, 5, status->cpu_core_utilization(i));
        queue_sql_statement(cpustmt);
    }

    for(int i = 0; i < status->processes_size(); i++){

        sqlite3_stmt *procstmt = get_sql_statement(get_process_insert_query());
        ProcessStatus proc = status->processes(i);

        if(proc.has_info()){
            sqlite3_stmt *procinfo = get_sql_statement(get_process_info_insert_query());
            sqlite3_bind_text(procinfo, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
            sqlite3_bind_int(procinfo, 2, message_id);
            sqlite3_bind_double(procinfo, 3, timestamp);
            sqlite3_bind_int(procinfo, 4, proc.pid());
            sqlite3_bind_text(procinfo, 5, proc.info().name().c_str(), proc.info().name().size(), SQLITE_TRANSIENT);
            sqlite3_bind_text(procinfo, 6, proc.info().args().c_str(), proc.info().args().size(), SQLITE_TRANSIENT);
            sqlite3_bind_int(procinfo, 7, proc.info().start_time());    
            queue_sql_statement(procinfo);
        }
        sqlite3_bind_text(procstmt, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
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
        sqlite3_bind_text(procstmt, 12, procstate.c_str(), procstate.size(), SQLITE_TRANSIENT);
        queue_sql_statement(procstmt);
    }

    for(int i = 0; i < status->interfaces_size(); i++){
        sqlite3_stmt *ifstatement = get_sql_statement(get_interface_insert_query());
        InterfaceStatus ifstat = status->interfaces(i);
        std::cout << ifstat.rx_bytes() << std::endl;

        if(ifstat.has_info()){
            sqlite3_stmt *ifinfo = get_sql_statement(get_interface_info_insert_query());
            sqlite3_bind_text(ifinfo, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
            sqlite3_bind_int(ifinfo, 2, message_id);
            sqlite3_bind_double(ifinfo, 3, timestamp);
            sqlite3_bind_text(ifinfo, 4, ifstat.name().c_str(), ifstat.name().size(), SQLITE_TRANSIENT);
            sqlite3_bind_text(ifinfo, 5, ifstat.info().type().c_str(), ifstat.info().type().size(), SQLITE_TRANSIENT);
            sqlite3_bind_text(ifinfo, 6, ifstat.info().description().c_str(), ifstat.info().description().size(), SQLITE_TRANSIENT);
            sqlite3_bind_text(ifinfo, 7, ifstat.info().ipv4_addr().c_str(), ifstat.info().ipv4_addr().size(), SQLITE_TRANSIENT);
            sqlite3_bind_text(ifinfo, 8, ifstat.info().ipv6_addr().c_str(), ifstat.info().ipv6_addr().size(), SQLITE_TRANSIENT);
            sqlite3_bind_text(ifinfo, 9, ifstat.info().mac_addr().c_str(), ifstat.info().mac_addr().size(), SQLITE_TRANSIENT);
            sqlite3_bind_int(ifinfo, 10, ifstat.info().speed());
            queue_sql_statement(ifinfo);
        }
        sqlite3_bind_text(ifstatement, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
        sqlite3_bind_int(ifstatement, 2, message_id);
        sqlite3_bind_double(ifstatement, 3, timestamp);
        sqlite3_bind_text(ifstatement, 4, ifstat.name().c_str(), ifstat.name().size(), SQLITE_TRANSIENT);
        sqlite3_bind_int(ifstatement, 5, ifstat.rx_packets());
        sqlite3_bind_int(ifstatement, 6, ifstat.rx_bytes());
        sqlite3_bind_int(ifstatement, 7, ifstat.tx_packets());
        sqlite3_bind_int(ifstatement, 8, ifstat.tx_bytes());
        queue_sql_statement(ifstatement);
    }

    for(int i = 0; i < status->file_systems_size(); i++){
        sqlite3_stmt *fsstatement = get_sql_statement(get_fs_insert_query());
        FileSystemStatus fss = status->file_systems(i);

        if(fss.has_info()){
            sqlite3_stmt *fsinfo = get_sql_statement(get_fs_info_insert_query());
            sqlite3_bind_text(fsinfo, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
            sqlite3_bind_int(fsinfo, 2, message_id);
            sqlite3_bind_double(fsinfo, 3, timestamp);
            sqlite3_bind_text(fsinfo, 4, fss.name().c_str(), fss.name().size(), SQLITE_TRANSIENT);
            std::string type = fs_type_to_string(fss.info().type());
            sqlite3_bind_text(fsinfo, 5, type.c_str(), type.size(), SQLITE_TRANSIENT);
            sqlite3_bind_int(fsinfo, 6, fss.info().size());
            queue_sql_statement(fsinfo);
        }
        sqlite3_bind_text(fsstatement, 1, hostname.c_str(), hostname.size(), SQLITE_TRANSIENT);
        sqlite3_bind_int(fsstatement, 2, message_id);
        sqlite3_bind_double(fsstatement, 3, timestamp);
        sqlite3_bind_text(fsstatement, 4, fss.name().c_str(), fss.name().size(), SQLITE_TRANSIENT);
        sqlite3_bind_int(fsstatement, 5, fss.utilization());
        queue_sql_statement(fsstatement);
    }

}

std::string LogDatabase::process_state_to_string(ProcessStatus::State state){
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

std::string LogDatabase::fs_type_to_string(FileSystemStatus::FileSystemInfo::Type type){
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