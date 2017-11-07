#ifndef CORE_NODEMANAGER_ZMQ_REGISTRAR_H
#define CORE_NODEMANAGER_ZMQ_REGISTRAR_H

#include <string>
#include <thread>
#include <vector>


class ExecutionManager;

namespace zmq{
    class context_t;

    class Registrar{
        public:
            Registrar(ExecutionManager* manager, std::string publisher_endpoint);
            ~Registrar();
        private:
            void RegistrationLoop(std::string endpoint);
            std::vector<std::string> slaves_;

            std::string publisher_endpoint_;
            ExecutionManager* execution_manager_;
            std::vector<std::thread*> registration_threads_;
            zmq::context_t* context_ = 0;
    };
};

#endif //CORE_NODEMANAGER_ZMQ_REGISTRAR_H