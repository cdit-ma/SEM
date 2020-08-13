#ifndef LOGAN_SERVER_PROTOHANDLERS_SYSTEMEVENT_H
#define LOGAN_SERVER_PROTOHANDLERS_SYSTEMEVENT_H

#ifdef _WIN32
#define NOMINMAX 1
#endif //_WIN32

#include <unordered_map>
#include <memory>
#include <set>
#include <mutex>

#include "protoreceiver.h"
#include "systemevent.pb.h"

#include "../../sqlitedatabase.h"
#include "../../protohandler.h"
#include "../../tableinsert.h"
#include "../../table.h"

namespace SystemEvent{    
    class ProtoHandler : public ::ProtoHandler{
        public:
            ProtoHandler(SQLiteDatabase& database);
            ~ProtoHandler();
            void BindCallbacks(zmq::ProtoReceiver& receiver);
        private:
            Table& GetTable(const std::string& table_name);
            bool GotTable(const std::string& table_name);

            void ExecuteTableStatement(TableInsert& row);

            //Table creation
            void CreateSystemStatusTable();
            void CreateSystemInfoTable();
            void CreateCpuTable();
            void CreateFileSystemTable();
            void CreateFileSystemInfoTable();
            void CreateInterfaceTable();
            void CreateInterfaceInfoTable();
            void CreateProcessTable();
            void CreateProcessInfoTable();

            //Callback functions
            void ProcessStatusEvent(const SystemEvent::StatusEvent& status);
            void ProcessInfoEvent(const SystemEvent::InfoEvent& info);

            //Add/Bind columns functions
            static void AddInfoColumns(Table& table);
            static void BindInfoColumns(TableInsert& row, const std::string& time, const std::string& host_name, const int64_t message_id);

            std::mutex mutex_;
            uint64_t rx_count_ = 0;

            SQLiteDatabase& database_;
            std::unordered_map<std::string, std::unique_ptr<Table> > tables_;
            std::set<std::string> registered_nodes_;
    };
};

#endif //LOGAN_SERVER_HARDWAREPROTOHANDLER_H
