#ifndef QPID_OUTEVENTPORT_H
#define QPID_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"


#include <vector>
#include <iostream>
#include <string>
#include <mutex>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Message_io.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Duration.h>

namespace qpid{
    template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::string name);
            void tx(T* message);

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();

            bool Passivate();

        private:
            void setup_tx();
            std::mutex control_mutex_;

            std::string broker_;
            std::string topic_;

            qpid::messaging::Connection connection_;
            qpid::messaging::Sender sender_;
    };
};

template <class T, class S>
void qpid::OutEventPort<T, S>::tx(T* message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(this->is_active()){
        auto str = proto::encode(message);
        qpid::messaging::Message m;
        m.setContentObject(str);
        sender_.send(m);
        ::OutEventPort<T>::tx(message);
    }
};

template <class T, class S>
qpid::OutEventPort<T, S>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name, "qpid"){};

template <class T, class S>
void qpid::OutEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("broker") && attributes.count("topic_name")){
        broker_ = attributes["broker"]->get_String();
        topic_ = attributes["topic_name"]->get_String();
        setup_tx();
    }

};

template <class T, class S>
void qpid::OutEventPort<T, S>::setup_tx(){
    connection_ = qpid::messaging::Connection(broker_);
    connection_.open();
    auto session = connection_.createSession();
    std::string tn = "amq.topic/" + topic_;
    sender_ = session.createSender(tn);
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::Teardown()){
        return true;
    }
    return false;
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(this->is_active()){
        if(connection_.isOpen()){
            connection_.close();
            connection_ = 0;
        }
    }
    return ::OutEventPort<T>::Passivate();
};

#endif //QPID_OUTEVENTPORT_H
