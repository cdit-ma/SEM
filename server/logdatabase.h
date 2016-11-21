#ifndef LOGDATABASE_H
#define LOGDATABASE_H

#include "SystemStatus.pb.h"
#include "sqlitedatabase.h"

class LogDatabase : public SQLiteDatabase{
    public:
    LogDatabase(std::string databaseFilepath);

    void process_status(SystemStatus* status);

    //System status and info tables
    std::string get_system_status_table_string() const;
    std::string get_system_status_insert_query() const;

    std::string get_system_info_table_string() const;
    std::string get_system_info_insert_query() const;

    //CPU core table
    std::string get_cpu_table_string() const;
    std::string get_cpu_insert_query() const;

    //FS tables
    std::string get_fs_table_string() const;
    std::string get_fs_insert_query() const;

    std::string get_fs_info_table_string() const;
    std::string get_fs_info_insert_query() const;

    //Network interface tables
    std::string get_interface_table_string() const;
    std::string get_interface_insert_query() const;

    std::string get_interface_info_table_string() const;
    std::string get_interface_info_insert_query() const;

    //Process table
    std::string get_process_table_string() const;
    std::string get_process_insert_query() const;

    std::string get_process_info_table_string() const;
    std::string get_process_info_insert_query() const;

    private:
    std::string process_state_to_string(ProcessStatus::State state);
    std::string fs_type_to_string(FileSystemStatus::FileSystemInfo::Type type);

};


#endif //LOGDATABASE_H