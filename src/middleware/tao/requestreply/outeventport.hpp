#ifndef TAO_OUTEVENTPORT_H
#define TAO_OUTEVENTPORT_H

#include <core/eventports/outeventport.hpp>
#include <string>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <middleware/tao/helper.h>
#include <set>

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
            R* get_writer(const std::string& endpoint);
            bool setup_tx();
            std::mutex control_mutex_;

            ::Base::Translator<T, S> translater;
            
            CORBA::ORB_var orb_ = 0;
            std::shared_ptr<Attribute> end_points_;
            std::shared_ptr<Attribute> publisher_names_;
            std::shared_ptr<Attribute> orb_endpoint_;

            
            std::set<std::string> registered_references_;

            std::unordered_map<std::string, CORBA::Object_ptr> object_ptrs_;
            std::unordered_map<std::string, R*> writers_;
    }; 
};

template <class T, class S, class R>
tao::OutEventPort<T, S, R>::OutEventPort(std::weak_ptr<Component> component, std::string name): ::OutEventPort<T>(component, name, "tao"){
    end_points_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_address").lock();
    publisher_names_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "publisher_names").lock();
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = orb_endpoint_->String().size() > 0 && end_points_->StringList().size() > 0 && publisher_names_->StringList().size() > 0;

    if(valid && ::OutEventPort<T>::HandleConfigure()){
        return setup_tx();
    }else{
        std::cerr << " CANNOT CONSTRUCT MATE: " <<orb_endpoint_->String() << " " << end_points_->StringList().size() << " " << publisher_names_->StringList().size() << std::endl;
        }
    return false;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::HandlePassivate()){
        registered_references_.clear();
        writers_.clear();
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
R* tao::OutEventPort<T, S, R>::get_writer(const std::string& name){
    auto& helper = tao::TaoHelper::get_tao_helper();
    R* writer = 0;

    if(registered_references_.count(name)){
        writer = writers_[name];
    }else{

        try{
            auto ptr = helper.resolve_initial_references(orb_, name);
            if(ptr){
                object_ptrs_[name] = ptr;
                writer = R::_narrow(ptr);;
                writers_[name] = writer;
                registered_references_.insert(name);
                std::cout << "Registered: " << name << std::endl;
            }
        }catch(...){
        }
    }
    return writer;
}

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::setup_tx(){
    auto orb_endpoint  = orb_endpoint_->String();
    auto& helper = tao::TaoHelper::get_tao_helper();
    std::cout << "OUT ORB ENDPOINT: " << orb_endpoint << std::endl;
    orb_ = helper.get_orb(orb_endpoint);
    std::cout << "OUT ORB ENDPOINT: " << orb_endpoint << std::endl;

    auto endpoints = end_points_->StringList();
    auto names = publisher_names_->StringList();

    for(int i = 0 ; i < endpoints.size(); i ++){
        auto endpoint = endpoints[i];
        auto name = names[i];
        std::cerr << "Registering: " << name << " to addr " << endpoint << std::endl;
        helper.register_initial_reference(orb_, name, endpoint);
        std::cerr << "Registered: " << name << " to addr " << endpoint << std::endl;
    }

    std::cerr  << orb_ << std::endl;
    return true;
    //return orb_;
};

template <class T, class S, class R>
bool tao::OutEventPort<T, S, R>::tx(const T& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::OutEventPort<T>::tx(message);

    if(should_send){
        //Send the mesasge 
        auto tao_message = translater.BaseToMiddleware(message);
        
        for(auto name : publisher_names_->StringList()){
            try{
                auto writer = get_writer(name);
                if(writer){
                    std::cout << "SENDING" << std::endl;
                    writer->send(*tao_message);
                    std::cout << "SENT" << std::endl;
                }else{
                    std::cerr << "Just waiting for a mate: " << name << std::endl;
                }
            }catch(...){
                registered_references_.erase(name);
            }
        }
        delete tao_message;
    }
    return false;
};



#endif //TAO_OUTEVENTPORT_H
