#ifndef LOGAN_SERVER_LOGPROTOHANDLER_H
#define LOGAN_SERVER_LOGPROTOHANDLER_H

#include <string>

#include "../re_common/proto/systemstatus/systemstatus.pb.h"

#include "../re_common/proto/modelevent/modelevent.pb.h"
#include "sqlitedatabase.h"
#include "zmqreceiver.h"

class Table;

class LogProtoHandler{
    public:
        LogProtoHandler(ZMQReceiver* receiver, SQLiteDatabase* database);
        void SetDatabase(SQLiteDatabase* database);

        void Process(google::protobuf::MessageLite* message);

        void ProcessSystemStatus(SystemStatus* status);
        void ProcessLifecycleEvent(re_common::LifecycleEvent* event);
        void ProcessMessageEvent(re_common::MessageEvent* event);
        void ProcessUserEvent(re_common::UserEvent* event);


    private:
        ZMQReceiver* receiver_;
        SQLiteDatabase* database_;

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
        void MessageEventTable();
        void UserEventTable();

};


#endif //LOGAN_SERVER_LOGPROTOHANDLER_H