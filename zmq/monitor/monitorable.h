#ifndef RE_COMMON_ZMQ_MONITORABLE_H
#define RE_COMMON_ZMQ_MONITORABLE_H

#include <mutex>
#include <string>

namespace zmq{
    class Monitor;

    class Monitorable{
        public:
            virtual void AttachMonitor(zmq::Monitor* monitor, int event_type) = 0;
            std::string GetNewMonitorAddress();
        private:
            int GetNewMonitorID();    
            static int count_;
            static std::mutex mutex_;
    };
}
#endif //RE_COMMON_ZMQ_MONITORABLE_H