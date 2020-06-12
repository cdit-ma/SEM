#ifndef LOGAN_SERVER_SERVER_H
#define LOGAN_SERVER_SERVER_H

#include <mutex>
#include <string>
#include <vector>
#include <memory>

class ProtoHandler;
class SQLiteDatabase;
namespace zmq{class ProtoReceiver;}

class Server{
    public:
        Server(const std::string& database_path, const std::vector<std::string>& addresses);
        ~Server();
        SQLiteDatabase& GetDatabase();
        void AddProtoHandler(std::unique_ptr<ProtoHandler> proto_handler);
    private:
        std::mutex mutex_;
        std::unique_ptr<SQLiteDatabase> database_;
        std::unique_ptr<zmq::ProtoReceiver> proto_receiver_;
        
        std::vector< std::unique_ptr<ProtoHandler> > proto_handlers_;
};

#endif //LOGAN_SERVER_SERVER_H
