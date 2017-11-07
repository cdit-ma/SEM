#include "activatable.h"
#include "modellogger.h"

const std::string Activatable::get_name(){
    return name_;
}

void Activatable::set_name(std::string name){
    name_ = name;
}

const std::string Activatable::get_id(){
    return id_;
}

void Activatable::set_id(std::string id){
    id_ = id;
}

const std::string Activatable::get_type(){
    return type_;
}

void Activatable::set_type(std::string type){
    type_ = type;
}

bool Activatable::Activate(){
    std::unique_lock<std::mutex> lock(state_mutex_);
    if(state_ == STATE::PASSIVE){
        state_ = STATE::ACTIVE;
        activate_condition_.notify_all();
        return true;
    }
    return false;
}

bool Activatable::Passivate(){
    std::unique_lock<std::mutex> lock(state_mutex_);
    if(state_ == STATE::ACTIVE){
        state_ = STATE::PASSIVE;
        return true;
    }
    return false;
}

bool Activatable::Teardown(){
    std::unique_lock<std::mutex> lock(state_mutex_);
    if(state_ == STATE::PASSIVE){
        state_ = STATE::DEAD;
        return true;
    }
    return false;
}

Activatable::STATE Activatable::get_state(){
    std::unique_lock<std::mutex> lock(state_mutex_);
    return state_;
}

bool Activatable::is_active(){
    std::unique_lock<std::mutex> lock(state_mutex_);
    return state_ == STATE::ACTIVE;
}

ModelLogger* Activatable::logger(){
    return ModelLogger::get_model_logger();
};

void Activatable::StartupFinished(){
    {
        //Flip our startup flag
        std::unique_lock<std::mutex> lock(startup_mutex_);
        startup_finished_ = true;
    }
    //Notify
    startup_condition_.notify_all();
}

void Activatable::WaitForActivate(){
    //Aquire a lock
    std::unique_lock<std::mutex> lock(state_mutex_);
    activate_condition_.wait(lock, [this]{return state_ == STATE::ACTIVE;});
}
void Activatable::WaitForStartup(){
    //Aquire a lock
    std::unique_lock<std::mutex> lock(startup_mutex_);
    startup_condition_.wait(lock, [this]{return this->startup_finished_;});
}

Activatable::~Activatable(){
}
        
std::shared_ptr<Attribute> Activatable::AddAttribute(std::shared_ptr<Attribute> attribute){
    std::lock_guard<std::mutex> lock(attributes_mutex_);
    if(attribute){
        auto name = attribute->get_name();
        if(attributes_.count(name) == 0){
            attributes_[name] = attribute;
            return attribute;
        }
    }
    attribute.reset();
    return attribute;
}

std::shared_ptr<Attribute> Activatable::GetAttribute(std::string name){
    std::lock_guard<std::mutex> lock(attributes_mutex_);
    std::shared_ptr<Attribute> attribute;
    
    if(attributes_.count(name)){
        attribute = attributes_[name];
    }
    return attribute;
}
