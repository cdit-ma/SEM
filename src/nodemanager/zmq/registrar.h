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
            Registrar(ExecutionManager& manager);
            ~Registrar();
        private:
            void RegistrationLoop();


            ExecutionManager& execution_manager_;
            std::future<void> registration_loop_;
            zmq::context_t* context_ = 0;
    };
};

#endif //CORE_NODEMANAGER_ZMQ_REGISTRAR_H