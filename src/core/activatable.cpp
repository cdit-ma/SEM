#include "activatable.h"
#include "modellogger.h"

const std::string Activatable::get_name(){
    return name_;
}

void Activatable::set_name(std::string name){
    name_ = name;
}
bool Activatable::Activate(){
    active_ = true;
    return true;
}

bool Activatable::Passivate(){
    active_ = false;
    return true;
}

const bool Activatable::is_active(){
    return active_;
}

ModelLogger* Activatable::logger(){
    return ModelLogger::get_model_logger();
};