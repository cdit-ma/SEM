#ifndef TAO_OUTEVENTPORT_H
#define TAO_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"
#include "../../core/modellogger.h"
#include <string>
#include <sstream>
#include <mutex>
#include <iostream>
#include <list>
#include <thread>

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
            void setup_loop();
            std::thread* setup_thread_ = 0;

            std::mutex control_mutex_;
            std::mutex writers_mutex_;

            bool configured_ = false;
            bool activated_ = false;

            std::vector<std::string> end_points_;
            std::list<std::string> references_;

            std::list<R*> writers_;
            CORBA::ORB_var orb_;
    }; 
};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::tx(T* message){
    if(this->is_active()){
        //Try and write message to each writer
        for(auto writer : writers_){
            try{
                //Send the mesasge 
                auto tao_message = translate(message);
                writer->send(*tao_message);
                delete tao_message;
            }catch(...){
                //Probably a non-connected writer
            }
        }
        ::OutEventPort<T>::tx(message);
    }
};

template <class T, class S, class R>
tao::OutEventPort<T, S, R>::OutEventPort(Component* component, std::string name):
::OutEventPort<T>(component, name, "tao"){
    //end_points_ = {"TestServer=corbaloc:iiop:192.168.111.90:50002/TestServer", "LoggingServer2=corbaloc:iiop:192.168.111.90:50003/LoggingServer2"};
    //references_ = {"TestServer", "LoggingServer2"};
};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    references_.clear();
    end_points_.clear();

    if(attributes.count("publisher_address")){
        for(auto s : attributes["publisher_address"]->StringList()){
            end_points_.push_back(s);
        }
    }

    if(attributes.count("publisher_references")){
        for(auto s : attributes["publisher_references"]->StringList()){
            references_.push_back(s);
        }
    }
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Teardown(){
    Passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
    return ::OutEventPort<T>::Teardown();
};

template <class T, class S, class R>
void tao::OutEventPort<T, S, R>::setup_loop(){
    //Construct a unique ID
    std::stringstream ss;
    ss << "tao::OutEventPort" << "_" << std::this_thread::get_id();
    auto unique_id = ss.str();

    //Construct the args for the TAO orb
    int orb_argc = 0;
    auto orb_argv = new char*[end_points_.size() * 2];

    for(auto &end_point : end_points_){
        orb_argv[orb_argc++] = (char *) "-ORBInitRef";
        orb_argv[orb_argc++] = &(end_point[0]);
    }

    //Initialize the orb with our custom endpoints
    orb_ = CORBA::ORB_init (orb_argc, orb_argv, unique_id.c_str());

    //Get the reference to the RootPOA
    auto root_poa_ref = orb_->resolve_initial_references("RootPOA");
    auto root_poa = ::PortableServer::POA::_narrow(root_poa_ref);
  
    while(true){
        for(auto reference : references_){
            try{
                //Get a reference
                auto ref_obj = orb_->resolve_initial_references(reference.c_str());
                if(ref_obj){
                    //Convert the ref_obj into a typed writer
                    auto writer_ = R::_narrow(ref_obj);
                    if(writer_){
                        //Gain the mutex and push back onto the queue of writers
                        std::lock_guard<std::mutex> lock(writers_mutex_);
                        writers_.push_back(writer_);
                        //Remove the 
                        references_.remove(reference);
                        break;
                    }
                }
            }
            catch(...){
                //Got an exception means we are probably trying to connect to a not yet existant end point.
            }
        }
        //Sleep for a while
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::Activate()){
        //Only construct if we aren't activated.
        setup_thread_ = new std::thread(&tao::OutEventPort<T, S, R>::setup_loop, this);
        return true;
    }
    return false;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::Passivate()){
        //Gain the writers mutex.
        std::lock_guard<std::mutex> lock(writers_mutex_);

        writers_.clear();
        //Destroying the orb will free the writers
        orb_->destroy();

        return true;
    }
    return false;
};

#endif //TAO_OUTEVENTPORT_H
