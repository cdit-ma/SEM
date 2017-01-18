#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H


#include <thread>
#include <string>
#include "graphmlparser.h"
class ZMQMaster;
class ExecutionManager{
    public:
        ExecutionManager(ZMQMaster *zmqmaster, std::string graphml_path);

        void execution_loop();
    private:
        
        std::thread* execution_thread_;
        ZMQMaster* zmq_master_;
        GraphmlParser* graphml_parser_;

};

#endif //EXECUTIONMANAGER_H