#ifndef RE_COMMON_ZMQ_REGISTRAR_REGISTRANT_H
#define RE_COMMON_ZMQ_REGISTRAR_REGISTRANT_H

#include <string>
#include <thread>
#include <queue>
#include <functional>
#include <vector>

namespace zmq{
	class socket_t;
	class context_t;
}

namespace zmq{
    class Registrant{
        public:
            Registrant(std::string request_message);
            ~Registrant();
            void AddEndpoint(std::string endpoint);
            void RegisterNotify(std::function<void(std::string, std::string)> fn);
            void Start();
        private:
            void RegistrationThread(std::string endpoint);
            
            std::function<void(std::string, std::string)> callback_ = nullptr;
            
            std::string request_message_;
            

            std::mutex mutex_;
            std::vector<std::string> endpoints_;
            
            std::queue<std::thread*> thread_queue_;

            std::thread* register_thread_ = 0;
            zmq::context_t* context_ = 0;
    };
}


#endif //RE_COMMON_ZMQ_REGISTRAR_REGISTRANT_H