#ifndef LOGAN_SERVER_LOGPROTOHANDLER_H
#define LOGAN_SERVER_LOGPROTOHANDLER_H

#include <string>
#include <map>
#include <set>
#include "sqlite3.h"

#include <google/protobuf/message_lite.h>
class Table;
class ZMQReceiver;
class SQLiteDatabase;
namespace google { namespace protobuf { class MessageLite; } }

class LogProtoHandler{
    public:
        LogProtoHandler(ZMQReceiver* receiver, SQLiteDatabase* database);
        ~LogProtoHandler();
        void ClientConnected(std::string topic_filter, std::string client_endpoint);
    private:
        void ProcessSystemStatus(google::protobuf::MessageLite* status);
        void ProcessOneTimeSystemInfo(google::protobuf::MessageLite* info);

        void ProcessLifecycleEvent(google::protobuf::MessageLite* message);
        void ProcessMessageEvent(google::protobuf::MessageLite* message);
        void ProcessUserEvent(google::protobuf::MessageLite* message);
        void ProcessWorkloadEvent(google::protobuf::MessageLite* message);
        void ProcessClientEvent(std::string client_endpoint);

        ZMQReceiver* receiver_;
        SQLiteDatabase* database_;

        std::map<std::string, Table*> table_map_;

        std::set<std::string> registered_nodes_;

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
        void CreateClientTable();
};
#endif //LOGAN_SERVER_LOGPROTOHANDLER_H
