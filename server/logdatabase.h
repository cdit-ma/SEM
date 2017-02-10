#ifndef LOGDATABASE_H
#define LOGDATABASE_H

#include <string>

#include "../re_common/proto/systemstatus/systemstatus.pb.h"

#include "../re_common/proto/modelevent/modelevent.pb.h"
#include "sqlitedatabase.h"
#include "table.h"


class LogDatabase : public SQLiteDatabase{
    public:
        LogDatabase(std::string databaseFilepath);

        void ProcessSystemStatus(SystemStatus* status);

        void ProcessLifecycleEvent(re_common::LifecycleEvent* event);
        void test(re_common::LifecycleEvent* event);


    private:

        std::map<std::string, Table*> table_map_;

        void SystemStatusTable();
        void SystemInfoTable();

        void CpuTable();
        void FileSystemTable();
        void FileSystemInfoTable();
        void InterfaceTable();
        void InterfaceInfoTable();
        void ProcessTable();
        void ProcessInfoTable();
        void PortEventTable();
        void ComponentEventTable();

        //Enum converters
        std::string process_state_to_string(const ProcessStatus::State state) const;
        std::string fs_type_to_string(const FileSystemStatus::FileSystemInfo::Type type) const;
        //std::string action_type_to_string(const ModelEvent::ActionType type) const;
        //std::string port_type_to_string(const ModelEvent::PortEvent::PortType type) const;

        int bind_string(sqlite3_stmt* stmnt, int pos, std::string str);

};


#endif //LOGDATABASE_H