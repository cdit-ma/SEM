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
    active_ = true;
    return true;
}

bool Activatable::Passivate(){
    active_ = false;
    return true;
}

bool Activatable::Teardown(){
    return true;
}

const bool Activatable::is_active(){
    return active_;
}

ModelLogger* Activatable::logger(){
    return ModelLogger::get_model_logger();
};