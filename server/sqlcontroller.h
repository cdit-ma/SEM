#ifndef SQLCONTROLLER_H
#define SQLCONTROLLER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include "zmq.hpp"
#include "logdatabase.h"
#include "systemstatus.pb.h"


class SQLController{
    public:
        SQLController(zmq::context_t *context);
        ~SQLController();
        void RecieverThread();
        
    private:
        void SQLThread();

        std::thread* reciever_thread_;
        std::thread* sql_thread_;

        LogDatabase* log_database;
        zmq::context_t *context_;
     
        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<std::string> rx_message_queue_; 
        
        bool terminate_;
};

#endif //SQLCONTROLLER_H