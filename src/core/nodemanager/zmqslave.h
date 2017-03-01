#ifndef ZMQSLAVE_H
#define ZMQSLAVE_H

#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <zmq.hpp>
#include "deploymentmanager.h"

class ZMQSlave{
    public:
        ZMQSlave(DeploymentManager* manager, std::string endpoint);
        ~ZMQSlave();
    private:
        void RegistrationLoop(std::string endpoint);

        DeploymentManager* deployment_manager_ = 0;

        std::thread* registration_thread_ = 0;
        zmq::context_t* context_ = 0;
};

#endif //ZMQSLAVE_H