#include "activatable.h"


const std::string Activatable::get_name(){
    return name_;
}

void Activatable::set_name(std::string name){
    name_ = name;
}
bool Activatable::activate(){
    active_ = true;
    return true;
}

bool Activatable::passivate(){
    active_ = false;
    return true;
}

const bool Activatable::is_active(){
    return active_;
}