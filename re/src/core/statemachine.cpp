#include "statemachine.h"
#include <stdexcept>
#include <iostream>

const std::string& StateMachine::ToString(const StateMachine::Transition& transition)
{
    const static std::unordered_map<StateMachine::Transition, std::string, EnumClassHash> lookup{
        {Transition::NO_TRANSITION, "NO_TRANSITION"},
        {Transition::CONFIGURE, "CONFIGURE"},
        {Transition::ACTIVATE, "ACTIVATE"},
        {Transition::PASSIVATE, "PASSIVATE"},
        {Transition::TERMINATE, "TERMINATE"}
    };
    try{
        return lookup.at(transition);
    }catch(const std::exception&){
        const static std::string error{"Invalid Transition"};
        return error; 
    }
}

const std::string& StateMachine::ToString(const StateMachine::State& state)
{
    const static std::unordered_map<StateMachine::State, std::string, EnumClassHash> lookup{
        {State::NOT_CONFIGURED, "NOT_CONFIGURED"},
        {State::CONFIGURED, "CONFIGURED"},
        {State::RUNNING, "RUNNING"},
        {State::NOT_RUNNING, "NOT_RUNNING"}
    };
    try{
        return lookup.at(state);
    }catch(const std::exception&){
        const static std::string error{"Invalid state"};
        return error; 
    }
}

StateMachine::State StateMachine::get_state() const
{
    return state_.load();
}

StateMachine::Transition StateMachine::get_transition() const
{
    return transition_.load();
}

bool StateMachine::Configure()
{
    return transition_state(Transition::CONFIGURE);
}

bool StateMachine::Activate()
{
    return transition_state(Transition::ACTIVATE);
}

bool StateMachine::Passivate()
{
    return transition_state(Transition::PASSIVATE);
}

bool StateMachine::Terminate()
{
    return transition_state(Transition::TERMINATE);
}

void StateMachine::RegisterTransitionFunction(const Transition& transition, const std::function<void ()>& func)
{
    std::lock_guard<std::mutex> transition_lock(transition_mutex_);
    transition_map_[transition] = func;
}

bool StateMachine::transition_state(const Transition& transition)
{
    std::lock_guard<std::mutex> state_lock(state_mutex_);

    const auto& current_state = get_state();
    auto new_state = current_state;
    
    switch(current_state){
        case State::NOT_CONFIGURED:{
            switch(transition){
                case Transition::CONFIGURE:{
                    new_state = State::CONFIGURED;
                    break;
                }
                case Transition::TERMINATE:{
                    return false;
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
                case Transition::PASSIVATE:{
                    return false;
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
    }

    if(new_state != current_state){
        if(run_transition_function(transition)){
            //Update the State
            state_ = new_state;
            return true;
        }
    }
    return false;
}

bool StateMachine::run_transition_function(const Transition& transition)
{
    bool success = false;
    try{
        //Set the Transition flag
        transition_ = transition;
        
        std::lock_guard<std::mutex> transition_lock(transition_mutex_);
        auto func_itt = transition_map_.find(transition);
        if(func_itt != transition_map_.end()){
            func_itt->second();
        }
        success = true;
    }catch(const std::exception& ex){
        std::cerr << "Transition Exception: [" << ToString(transition) << "]: " << ex.what() << std::endl;
    }
    //Reset the transition
    transition_ = Transition::NO_TRANSITION;
    return success;
}