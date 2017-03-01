#ifndef ZMQSLAVE_H
#define ZMQSLAVE_H

#include <string>
#include <thread>

#include "deploymentmanager.h"

namespace zmq{class context_t;};

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