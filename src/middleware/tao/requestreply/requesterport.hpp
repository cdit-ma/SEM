#ifndef TAO_PORT_REQUESTER_HPP
#define TAO_PORT_REQUESTER_HPP

#include <core/ports/requestreply/requesterport.hpp>
#include <middleware/tao/taohelper.h>

#include <string>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <middleware/tao/helper.h>
#include <set>

namespace tao{


    //Generic templated RequesterPort
    template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
    class RequesterPort : public ::RequesterPort<BaseReplyType, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            };
            
            BaseReplyType ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);

            using middleware_reply_type = TaoReplyType;
            using middleware_request_type = TaoRequestType;
        protected:
            bool HandleConfigure();
            bool HandleTerminate();
            static bool SetupRequester(tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port, const std::string& orb_endpoint, const std::string& publisher_address, const std::string& publisher_name);
        private:
            std::shared_ptr<Attribute> end_points_;
            std::shared_ptr<Attribute> publisher_names_;
            std::shared_ptr<Attribute> orb_endpoint_;

            CORBA::ORB_var orb_ = 0;
            std::mutex client_mutex_;
            TaoClientImpl* tao_client_ = 0;
    };

    //Specialised templated RequesterPort for void returning
    /*template <class BaseRequestType, class TaoRequestType, class TaoClientImpl>
    class RequesterPort<void, void, BaseRequestType, TaoRequestType, TaoClientImpl>: public ::RequesterPort<BaseReplyType, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            };
            
            void ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);

            using middleware_reply_type = void;
            using middleware_request_type = TaoRequestType;
        private:
            std::shared_ptr<Attribute> end_points_;
            std::shared_ptr<Attribute> publisher_names_;
            std::shared_ptr<Attribute> orb_endpoint_;
    };*/
};


//Generic templated ReplierPort
template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, BaseRequestType>(component, port_name, "tao"){
    orb_endpoint_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "orb_endpoint").lock();
    publisher_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_name").lock();
    end_point_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "publisher_address").lock();
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    const auto orb_endpoint = orb_endpoint_->String();
    const auto publisher_name = publisher_name_->String();
    const auto publisher_address = end_point_->String();

    bool valid = orb_endpoint.size() > 0 && publisher_address.size() > 0 && publisher_name.size() > 0;

    if(valid && ::RequesterPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
        return SetupRequester(*this, orb_endpoint, publisher_address, publisher_name);
    }
    return false;
};


template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::RequesterPort<Base ReplyType, BaseRequestType>::HandleTerminate()){
        std::lock_guard<std::mutex> client_lock(client_mutex_);
        delete tao_client_;
        tao_client_ = 0;
        return true; 
    }
    return false;
};

template <class BaseReplyType, class TaoReplyType, class BaseRequestType, class TaoRequestType, class TaoClientImpl>
bool tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>::SetupRequester(
    tao::RequesterPort<BaseReplyType, TaoReplyType, BaseRequestType, TaoRequestType, TaoClientImpl>& port, 
    const std::string& orb_endpoint,
    const std::string& publisher_address,
    const std::string& publisher_name)
{
    std::lock_guard<std::mutex> client_lock(port.client_mutex_);
    auto& helper = tao::TaoHelper::get_tao_helper();

    port.orb_ = helper.get_orb(orb_endpoint);

    helper.register_initial_reference(port.orb_, publisher_name, publisher_endpoint);
    std::cerr << "Registered: " << publisher_name << " to addr " << publisher_endpoint << std::endl;
    auto& helper = tao::TaoHelper::get_tao_helper();
    
    try{
        auto ptr = helper.resolve_initial_references(port.orb_, publisher_name);
        if(ptr){
            port.tao_client_ = TaoClientImpl::_narrow(ptr);
            std::cout << "Registered: " << publisher_name << std::endl;
            return true;
        }catch(...){
            std::cerr << "ERROR";
        }
    }
    return false;
};



#endif //TAO_PORT_REQUESTER_HPP
