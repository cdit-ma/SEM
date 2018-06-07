#include "activatable.h"
#include "modellogger.h"
#include <iostream>
#include <typeinfo>

const std::string Activatable::ToString(const Activatable::Transition& transition){
    switch(transition){
        case Transition::NO_TRANSITION:
            return "NO_TRANSITION";
        case Transition::CONFIGURE:
            return "CONFIGURE";
        case Transition::ACTIVATE:
            return "ACTIVATE";
        case Transition::PASSIVATE:
            return "PASSIVATE";
        case Transition::TERMINATE:
            return "TERMINATE";
    }
};

const std::string Activatable::ToString(const Activatable::State& state){
    switch(state){
        case State::NOT_CONFIGURED:
            return "NOT_CONFIGURED";
        case State::CONFIGURED:
            return "CONFIGURED";
        case State::RUNNING:
            return "RUNNING";
        case State::NOT_RUNNING:
            return "NOT_RUNNING";
    }
};

std::string Activatable::get_name() const{
    return name_;
}

void Activatable::set_name(std::string name){
    name_ = name;
}

std::string Activatable::get_id() const{
    return id_;
}

void Activatable::set_id(std::string id){
    id_ = id;
}

std::string Activatable::get_type() const{
    return type_;
}

void Activatable::set_type(std::string type){
    type_ = type;
}

bool Activatable::Activate(){
    return transition_state(Transition::ACTIVATE);
}

bool Activatable::Passivate(){
    return transition_state(Transition::PASSIVATE);
}

bool Activatable::Terminate(){
    return transition_state(Transition::TERMINATE);
}

bool Activatable::Configure(){
    return transition_state(Transition::CONFIGURE);
}


bool Activatable::transition_state(Transition transition){
    std::unique_lock<std::mutex> lock(transition_mutex_);
    //get the current state
    State current_state = get_state();
    State new_state = current_state;
    switch(current_state){
        case State::NOT_CONFIGURED:{
            switch(transition){
                case Transition::CONFIGURE:{
                    new_state = State::CONFIGURED;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case State::CONFIGURED:{
            switch(transition){
                case Transition::ACTIVATE:{
                    new_state = State::RUNNING;
                    break;
                }
                case Transition::TERMINATE:{
                    new_state = State::NOT_CONFIGURED;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case State::RUNNING:{
            switch(transition){
                case Transition::PASSIVATE:{
                    new_state = State::NOT_RUNNING;
                    break;
                }
                case Transition::TERMINATE:{
                    new_state = State::NOT_CONFIGURED;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case State::NOT_RUNNING:{
            switch(transition){
                case Transition::TERMINATE:{
                    new_state = State::NOT_CONFIGURED;
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }




    if(new_state != current_state){
        //Notify
        {
            //Notify things blocking
            std::unique_lock<std::mutex> lock(state_mutex_);
            transition_ = transition;
            state_condition_.notify_all();
        }
        bool transitioned = false;
        switch(transition){
            case Transition::CONFIGURE:{
                transitioned = HandleConfigure();
                break;
            }
            case Transition::ACTIVATE:{
                transitioned = HandleActivate();
                break;
            }
            case Transition::PASSIVATE:{
                transitioned = HandlePassivate();
                break;
            }
            case Transition::TERMINATE:{
                transitioned = HandleTerminate();
                break;
            }
            default:
                break;
        }

        //If transitioned, move the state, otherwise fail
        if(transitioned){
            //Change state
            std::unique_lock<std::mutex> lock(state_mutex_);
            state_ = new_state;
            transition_ = Activatable::Transition::NO_TRANSITION;
            state_condition_.notify_all();
            return true;
        }else{
            Log(Severity::ERROR_).Context(this).Func(GET_FUNC).Msg(get_name() + ": Cannot transition from state: " + ToString(current_state) + " transition: " + ToString(transition));
            return false;
        }
    }else{
        Log(Severity::DEBUG).Context(this).Func(GET_FUNC).Msg(get_name() + " :Cannot transition from state: " + ToString(current_state) + " transition: " + ToString(transition));
    }
    return false;
}

Activatable::State Activatable::get_state(){
    std::unique_lock<std::mutex> lock(state_mutex_);
    return state_;
}

ModelLogger* Activatable::logger(){
    return ModelLogger::get_model_logger();
};

bool Activatable::is_running(){
   return get_state() == Activatable::State::RUNNING; 
}

bool Activatable::BlockUntilStateChanged(const Activatable::State desired_state){
    std::unique_lock<std::mutex> lock(state_mutex_);
    state_condition_.wait(lock, [this, desired_state]{return state_ == desired_state || transition_ == Activatable::Transition::TERMINATE;});
    
    //Terminate should always error
    if(transition_ == Activatable::Transition::TERMINATE){
        return false;
    }else{
        return state_ == desired_state;
    }
}

std::weak_ptr<Attribute> Activatable::AddAttribute(std::unique_ptr<Attribute> attribute){
    std::lock_guard<std::mutex> lock(attributes_mutex_);
    if(attribute){
        std::string name = attribute->get_name();
        if(attributes_.count(name) == 0){
            attributes_[name] = std::move(attribute);
            return attributes_[name];
        }else{
            Log(Severity::WARNING).Context(this).Func(GET_FUNC).Msg("Already got an Attribute with name '" + name + "'");
        }
    }else{
        Log(Severity::WARNING).Context(this).Func(GET_FUNC).Msg("Cannot attach a null Attribute");
    }
    return std::weak_ptr<Attribute>();
}

std::weak_ptr<Attribute> Activatable::ConstructAttribute(const ATTRIBUTE_TYPE type, const std::string name){
    //Try get the attribute, if it exists
    auto attribute_sp = GetAttribute(name).lock();
    if(attribute_sp){
        if(attribute_sp->get_type() != type){
            Log(Severity::WARNING).Context(this).Func(GET_FUNC).Msg("Already got an Attribute with name '" + name + "' with a differing type");
            attribute_sp.reset();
        }
    }else{
        attribute_sp = AddAttribute(std::unique_ptr<Attribute>(new Attribute(type, name))).lock();
    }
    return attribute_sp;
}

std::weak_ptr<Attribute> Activatable::GetAttribute(const std::string& name){
    std::lock_guard<std::mutex> lock(attributes_mutex_);
    if(attributes_.count(name)){
        return attributes_[name];
    }else{
        return std::weak_ptr<Attribute>();
    }
}