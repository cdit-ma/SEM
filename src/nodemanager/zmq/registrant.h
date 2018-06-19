#ifndef CORE_NODEMANAGER_ZMQ_REGISTRANT_H
#define CORE_NODEMANAGER_ZMQ_REGISTRANT_H

#include <string>
#include <mutex>
#include <future>

class DeploymentManager;

namespace zmq{
    class context_t;
    
    //Client
    class Registrant{
        public:
            Registrant(DeploymentManager& deployment_manager);
            ~Registrant();

            void SendTerminated();
        private:
            void RegistrationLoop();

            std::mutex terminated_mutex_;
            bool terminated_ = false;
            std::condition_variable terminated_cv_;



            DeploymentManager& deployment_manager_;
            std::future<void> registration_loop_;
            zmq::context_t* context_ = 0;
    };
};

#endif //CORE_NODEMANAGER_ZMQ_REGISTRANT_H