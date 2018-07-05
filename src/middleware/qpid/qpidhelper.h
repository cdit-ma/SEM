#ifndef QPID_QPIDHELPER_H
#define QPID_QPIDHELPER_H

namespace qpid{
    class QpidHelper{
        public:
            static QpidHelper& get_qpid_helper();
    };

    class PortHelper{
        public:
            PortHelper(const std::string& broker_endpoint);
            ~PortHelper();
            qpid::messaging::Sender GetSender();
            qpid::messaging::Receiver GetReceiver();
            void Terminate();
        private:
            std::mutex mutex_;
            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Session session_ = 0;
    };
};

#endif //QPID_QPIDHELPER_H