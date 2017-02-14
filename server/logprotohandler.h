#ifndef LOGAN_SERVER_LOGPROTOHANDLER_H
#define LOGAN_SERVER_LOGPROTOHANDLER_H

#include <string>

#include "../re_common/proto/systemstatus/systemstatus.pb.h"

#include "../re_common/proto/modelevent/modelevent.pb.h"
#include "../re_common/zmqprotoreceiver/zmqreceiver.h"
#include "sqlitedatabase.h"

class Table;

class LogProtoHandler{
    public:
        LogProtoHandler(ZMQReceiver* receiver, SQLiteDatabase* database);
        ~LogProtoHandler();

        void Process(google::protobuf::MessageLite* message);
        void ProcessSystemStatus(SystemStatus* status);
        void ProcessLifecycleEvent(re_common::LifecycleEvent* event);
        void ProcessMessageEvent(re_common::MessageEvent* event);
        void ProcessUserEvent(re_common::UserEvent* event);
        void ProcessWorkloadEvent(re_common::WorkloadEvent* event);


    private:
        ZMQReceiver* receiver_;
        SQLiteDatabase* database_;

        std::map<std::string, Table*> table_map_;

        void CreateSystemStatusTable();
        void CreateSystemInfoTable();
        void CreateCpuTable();
        void CreateFileSystemTable();
        void CreateFileSystemInfoTable();
        void CreateInterfaceTable();
        void CreateInterfaceInfoTable();
        void CreateProcessTable();
        void CreateProcessInfoTable();
        void CreatePortEventTable();
        void CreateComponentEventTable();
        void CreateMessageEventTable();
        void CreateUserEventTable();
        void CreateWorkloadEventTable();

};


#endif //LOGAN_SERVER_LOGPROTOHANDLER_H