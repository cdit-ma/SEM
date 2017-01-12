#include "activatable.h"

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