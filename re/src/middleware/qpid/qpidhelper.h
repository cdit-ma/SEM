#ifndef QPID_QPIDHELPER_H
#define QPID_QPIDHELPER_H

#include <string>
#include <mutex>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Duration.h>
#include <qpid/messaging/Sender.h>


namespace qpid{
    class QpidHelper{
        public:
            static QpidHelper& get_qpid_helper();
    };

    class PortHelper{
        public:
            PortHelper(const std::string& broker_endpoint);
            ~PortHelper();
            qpid::messaging::Sender GetSender(const std::string& address);
            qpid::messaging::Sender GetSender(const qpid::messaging::Address& address);
            qpid::messaging::Receiver GetReceiver(const std::string& address);
            qpid::messaging::Receiver GetReceiver(const qpid::messaging::Address& address);
            void Terminate();
        private:
            std::mutex mutex_;
            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Session session_ = 0;
    };
};

#endif //QPID_QPIDHELPER_H