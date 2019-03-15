
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

class StateMachine{
public:
    enum class Transition{
        NO_TRANSITION = 0,
        CONFIGURE = 1,
        ACTIVATE = 2,
        PASSIVATE = 3,
        TERMINATE = 4,
    };

    enum class State{
        NOT_CONFIGURED = 0,
        CONFIGURED = 1,
        RUNNING = 2,
        NOT_RUNNING = 3,
    };

    public:
        State get_state() const;
        Transition get_transition() const;

        bool Configure();
        bool Activate();
        bool Passivate();
        bool Terminate();

        void RegisterTransitionFunction(const Transition& transation, const std::function<void ()>& func);

        static const std::string& ToString(const Transition& transation);
        static const std::string& ToString(const State& transation);
    private:
        bool transition_state(const Transition& transition);
        bool run_transition_function(const Transition& transition);

        std::mutex transition_mutex_;
        std::unordered_map<Transition, std::function<void ()>, EnumClassHash> transition_map_;
        
        std::mutex state_mutex_;
        std::atomic<State> state_{State::NOT_CONFIGURED};
        std::atomic<Transition> transition_{Transition::NO_TRANSITION};
};


#endif //STATE_MACHINE_H