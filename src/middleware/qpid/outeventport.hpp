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

            bool Activate();
            bool Passivate();

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

        std::cout << "qpid::OutEventPort" << std::endl;
        std::cout << "**broker: "<< broker_ << std::endl;
        std::cout << "**topic_name: "<< topic_ << std::endl << std::endl;

    
        configured_ = true;
    }

};

template <class T, class S>
bool qpid::OutEventPort<T, S>::Teardown(){
    Passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
    return ::OutEventPort<T>::Teardown();
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(configured_){
        connection_ = qpid::messaging::Connection(broker_);
        connection_.open();
        session_ = connection_.createSession();
        std::string tn = "amq.topic/" + topic_;
        sender_ = session_.createSender(tn);
    }
    return ::OutEventPort<T>::Activate();    
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(connection_.isOpen()){
        connection_.close();
        std::cout << "out eventport passivate 1" << std::cout;
        connection_ = 0;
        std::cout << "out eventport passivate 2" << std::cout;
    }
    return ::OutEventPort<T>::Passivate();
};

#endif //QPID_OUTEVENTPORT_H
