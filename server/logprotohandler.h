#ifndef LOGAN_SERVER_LOGPROTOHANDLER_H
#define LOGAN_SERVER_LOGPROTOHANDLER_H

#include <string>
#include <map>

class Table;
class ZMQReceiver;
class SQLiteDatabase;
namespace google { namespace protobuf { class MessageLite; } }

class LogProtoHandler{
    public:
        LogProtoHandler(ZMQReceiver* receiver, SQLiteDatabase* database);
        ~LogProtoHandler();

    private:
        void ProcessSystemStatus(google::protobuf::MessageLite* status);
        void ProcessLifecycleEvent(google::protobuf::MessageLite* message);
        void ProcessMessageEvent(google::protobuf::MessageLite* message);
        void ProcessUserEvent(google::protobuf::MessageLite* message);
        void ProcessWorkloadEvent(google::protobuf::MessageLite* message);

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
