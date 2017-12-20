#ifndef CORE_NODEMANAGER_ZMQ_REGISTRANT_H
#define CORE_NODEMANAGER_ZMQ_REGISTRANT_H

#include <string>
#include <thread>

#include "../deploymentmanager.h"

namespace zmq{
    class context_t;
    
    class Registrant{
        public:
            Registrant(DeploymentManager* manager, std::string endpoint);
            ~Registrant();
        private:
            void RegistrationLoop(std::string endpoint);

            DeploymentManager* deployment_manager_ = 0;
            std::thread* registration_thread_ = 0;
            zmq::context_t* context_ = 0;
    };
};

#endif //CORE_NODEMANAGER_ZMQ_REGISTRANT_H