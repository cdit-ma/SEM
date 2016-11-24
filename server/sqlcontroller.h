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
        SQLController();
        ~SQLController();
        
        void terminate_reciever();
        
    private:
        void RecieverThread();
        void SQLThread();

        std::thread* reciever_thread_;
        std::thread* sql_thread_;

        zmq::socket_t *term_socket;

        LogDatabase* log_database;
        zmq::context_t *context_;
     
        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<std::string> rx_message_queue_; 
        
        bool terminate_;
};

#endif //SQLCONTROLLER_H