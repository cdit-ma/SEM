#ifndef RE_COMMON_ZMQ_REGISTRAR_REGISTRAR_H
#define RE_COMMON_ZMQ_REGISTRAR_REGISTRAR_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>

namespace zmq{
	class socket_t;
	class context_t;
}

namespace zmq{
    class Registrar{
        public:
            Registrar(std::string endpoint, std::string reply_message);
            ~Registrar();
            void Start();
            void RegisterNotify(std::function<void(std::string)> fn);
        private:
            void RegistrationThread();
            void NotifyThread();

            std::string endpoint_;
            std::string reply_message_;

            std::function<void(std::string)> callback_ = nullptr;

            std::condition_variable queue_lock_condition_;
            std::mutex queue_mutex_;
            std::queue<std::string> registered_endpoints_; 

            bool terminate_ = false;
            std::thread* register_thread_ = 0;
            std::thread* notify_thread_ = 0;
            zmq::context_t* context_ = 0;
            std::mutex mutex_;
    };
}


#endif //RE_COMMON_ZMQ_REGISTRAR_REGISTRAR_H