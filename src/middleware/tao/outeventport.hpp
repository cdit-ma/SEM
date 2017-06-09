#ifndef TAO_OUTEVENTPORT_H
#define TAO_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"
#include "../../core/modellogger.h"
#include <string>
#include <mutex>


#include "helper.hpp"

namespace tao{
     template <class T, class S, class R> class OutEventPort: public ::OutEventPort<T>{
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

            std::string object_key_;
            std::vector<std::string> end_points_;

            R* writer_ = 0;
    }; 
};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::tx(T* message){
    if(this->is_active() && writer_ != 0){
        auto m = tao::translate(message);
        //De-reference the message and send
        writer_->send(*m);
        delete m;
        ::OutEventPort<T>::tx(message);
    }
};

template <class T, class S, class R>
tao::OutEventPort<T, S, R>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name, "tao"){};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    std::cout << "tao::OutEventPort" << std::endl;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Teardown(){
    Passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
    return ::OutEventPort<T>::Teardown();
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    //Get a pointer to the orb
    auto orb = CORBA::ORB_init (argc, argv);

    //Get the reference to the RootPOA
    auto obj = orb->resolve_initial_references("RootPOA");
    auto root_poa = ::PortableServer::POA::_narrow(obj);

    R::_narrow(ref_obj);

    std::string reference_str = "LoggingServer";
    auto ref_obj = orb->resolve_initial_references(reference_str.c_str());

    if(!ref_obj){
        std::cerr << "Failed to resolve Reference '" << reference_str << "'" << std::endl;
    }

    //Convert the ref_obj into a typed writer
    writer_ = Test::Hello::_narrow(ref_obj);

    if(!writer_){
        std::cerr << "NILL REFERENCE Y'AL" << std::endl;
    }

    return ::OutEventPort<T>::Activate();
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    writer_ = 0;
    return ::OutEventPort<T>::Passivate();
};

#endif //TAO_OUTEVENTPORT_H
