#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include "zmqmaster.h"
#include <thread>
#include <string>

class ExecutionManager{
    public:
        ExecutionManager(ZMQMaster *zmqmaster, std::string graphml_path);


    private:
        void execution_loop();
        std::thread* execution_thread_;
        ZMQMaster* zmq_master_;
        GraphmlParser* graphml_parser_;

};

#endif //EXECUTIONMANAGER_H