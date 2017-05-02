#ifndef QPID_QPIDHELPER_H
#define QPID_QPIDHELPER_H

#include <string>
#include <mutex>

namespace qpid{
    class QpidHelper{
        public:
            //Static getter functions
            static QpidHelper* get_zmq_helper();
            static std::mutex global_mutex_;
        private:
            static QpidHelper* singleton_;
    };
};

#endif //QPID_QPIDHELPER_H

