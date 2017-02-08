#ifndef SQLCONTROLLER_H
#define SQLCONTROLLER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

#include "zmq.hpp"
#include "logdatabase.h"

class SQLController{
    public:
        SQLController(std::vector<std::string> addrs, std::string port, std::string file);
        ~SQLController();

        void TerminateReceiver();
        
    private:
        void RecieverThread();
        void SQLThread();
        
        std::vector<std::string> addresses_;
        std::string port_;

        std::thread* reciever_thread_;
        std::thread* sql_thread_;

        zmq::context_t *context_;
        zmq::socket_t *term_socket_;

        LogDatabase* log_database_;
        
             
        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<std::pair<std::string, std::string> > rx_message_queue_; 
        
        bool terminate_reciever_ = false;
        bool terminate_sql_ = false;
};

#endif //SQLCONTROLLER_H