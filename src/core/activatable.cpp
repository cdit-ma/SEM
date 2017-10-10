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
    if(state_ == STATE::PASSIVE){
        state_ = STATE::ACTIVE;
        return true;
    }
    return false;
}

bool Activatable::Passivate(){
    if(state_ == STATE::ACTIVE){
        state_ = STATE::PASSIVE;
        return true;
    }
    return false;
}

bool Activatable::Teardown(){
    if(state_ == STATE::PASSIVE){
        state_ = STATE::DEAD;
        return true;
    }
    return false;
}

const Activatable::STATE Activatable::get_state(){
    return state_;

}

const bool Activatable::is_active(){
    return state_ == STATE::ACTIVE;
}

ModelLogger* Activatable::logger(){
    return ModelLogger::get_model_logger();
};