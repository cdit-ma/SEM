#ifndef QPID_OUTEVENTPORT_H
#define QPID_OUTEVENTPORT_H

#include "../globalinterfaces.hpp"
#include <vector>
#include <iostream>
#include <string>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Message_io.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>


namespace qpid{
    template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(::OutEventPort<T>* port, std::string broker, std::string topic);
            void tx_(T* message);
        private:
            qpid::messaging::Connection connection_;
            qpid::messaging::Session session_;
            qpid::messaging::Sender sender_;

            ::OutEventPort<T>* port_;

            
    };
};

template <class T, class S>
void qpid::OutEventPort<T, S>::tx_(T* message){
    std::string str = proto::encode(message);

    qpid::messaging::Message m;
    m.setContentObject(str);

    sender_.send(m);
}


template <class T, class S>
qpid::OutEventPort<T, S>::OutEventPort(::OutEventPort<T>* port, std::string broker, std::string topic){
    this->port_ = port;

    //auto helper = QpidHelper::get_qpid_helper();

    connection_ = qpid::messaging::Connection(broker);
    connection_.open();
    session_ = connection_.createSession();
    std::string tn = "amq.topic/" + topic;
    sender_ = session_.createSender(tn);

}

#endif //QPID_OUTEVENTPORT_H