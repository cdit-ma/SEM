#ifndef QPID_OUTEVENTPORT_H
#define QPID_OUTEVENTPORT_H

#include "../core/eventports/outeventport.hpp"


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


namespace qpid{
    template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::string name);
            void tx(T* message);

            void startup(std::map<std::string, ::Attribute*> attributes);
            void teardown();

            bool activate();
            bool passivate();
        private:
            std::mutex control_mutex_;
            bool configured_ = false;

            std::string broker_;
            std::string topic_;

            qpid::messaging::Connection connection_;
            qpid::messaging::Session session_;
            qpid::messaging::Sender sender_;
    };
};

template <class T, class S>
void qpid::OutEventPort<T, S>::tx(T* message){
    if(this->is_active()){
        std::string str = proto::encode(message);

        qpid::messaging::Message m;
        m.setContentObject(str);

        sender_.send(m);
    }
};

template <class T, class S>
qpid::OutEventPort<T, S>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name){};

template <class T, class S>
void qpid::OutEventPort<T, S>::startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(attributes.count("broker") && attributes.count("topic_name")){
        broker_ = attributes["broker"]->get_string();
        topic_ = attributes["topic_name"]->get_string();
        configured_ = true;
    }

};

template <class T, class S>
void qpid::OutEventPort<T, S>::teardown(){
    passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(configured_){
        connection_ = qpid::messaging::Connection(broker_);
        connection_.open();
        session_ = connection_.createSession();
        std::string tn = "amq.topic/" + topic_;
        sender_ = session_.createSender(tn);
    }
    return ::OutEventPort<T>::activate();    
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(connection_.isOpen()){
        connection_.close();
        connection_ = 0;
    }
    return ::OutEventPort<T>::passivate();
 
};

#endif //QPID_OUTEVENTPORT_H