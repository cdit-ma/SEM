#ifndef CORE_NODEMANAGER_ZMQ_REGISTRAR_H
#define CORE_NODEMANAGER_ZMQ_REGISTRAR_H

#include <string>
#include <thread>
#include <list>
#include <future>


class ExecutionManager;

namespace zmq{
    class context_t;

    class Registrar{
        public:
            Registrar(ExecutionManager* manager, const std::string& publisher_endpoint);
            ~Registrar();
        private:
            bool RegistrationLoop(const std::string& endpoint);

            std::string publisher_endpoint_;
            ExecutionManager* execution_manager_;
            std::list<std::future<bool> > registration_results_;
            zmq::context_t* context_ = 0;
    };
};

#endif //CORE_NODEMANAGER_ZMQ_REGISTRAR_H