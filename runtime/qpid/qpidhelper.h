#ifndef QPIDHELPER_H
#define QPIDHELPER_H

#include "qpid/messaging/Connection.h"
#include "qpid/messaging/Message.h"
#include "qpid/messaging/Receiver.h"
#include "qpid/messaging/Sender.h"
#include "qpid/messaging/Session.h"

namespace qpid{
    class QpidHelper{
        public:
            static QpidHelper* get_qpid_helper();
            qpid::messaging::Connection* get_connection(std::string broker);

        private:
            static QpidHelper* qpid_singleton_;
            QpidHelper(){};
            ~QpidHelper(){};

    };
};


#endif //QPIDHELPER_H