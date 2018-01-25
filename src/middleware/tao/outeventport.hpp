#ifndef TAO_OUTEVENTPORT_H
#define TAO_OUTEVENTPORT_H

#include <core/eventports/outeventport.hpp>
#include <string>
#include <sstream>
#include <mutex>
#include <iostream>
#include <list>
#include <thread>

namespace tao{
     template <class T, class S, class R> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(std::weak_ptr<Component> component, std::string name);
            ~OutEventPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool tx(const T& message);
        private:
            bool setup_tx();
            std::mutex control_mutex_;
            
            CORBA::ORB_var orb_;
            std::shared_ptr<Attribute> end_points_;
            std::shared_ptr<Attribute> publisher_names_;

            std::list<R*> writers_;
    }; 
};

template <class T, class S, class R>
tao::OutEventPort<T, S, R>::OutEventPort(std::weak_ptr<Component> component, std::string name): ::OutEventPort<T>(component, name, "tao"){
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
    publisher_names_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_names").lock();
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = end_points_->StringList().size() > 0 && publisher_names_->StringList().size() > 0;

    if(valid && ::OutEventPort<T>::HandleConfigure()){
        return setup_tx();
    }
    return false;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::HandlePassivate()){
        writers_.clear();
        //Destroying the orb will free the writers
        orb_->destroy();
        return true;
    }
    return false;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::OutEventPort<T>::HandleTerminate();
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::setup_tx(){
    //Construct a unique ID
    std::stringstream ss;
    ss << "tao::OutEventPort" << "_" << std::this_thread::get_id();
    auto unique_id = ss.str();
    
    auto endpoints_  = end_points_->StringList();
    const auto references_  = publisher_names_->StringList();
    //Construct the args for the TAO orb
    int orb_argc = 0;
    auto orb_argv = new char*[endpoints_.size() * 2];

    for(auto &end_point : endpoints_){
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
                        //std::lock_guard<std::mutex> lock(writers_mutex_);
                        writers_.push_back(writer_);
                        //Remove the 
                        //references_.remove(reference);
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

    return true;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::tx(const T& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::OutEventPort<T>::tx(message);

    if(should_send){
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
    }
    return false;
};



#endif //TAO_OUTEVENTPORT_H
