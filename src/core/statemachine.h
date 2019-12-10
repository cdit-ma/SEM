
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

// REVIEW (Mitch): This state machine seems to be 50/50 configurable/hard coded. Review finds that
//  moving in a more hard coded direction would be profitable. Namespace and only use in context of
//  `re` activatables.
// REVIEW (Mitch): Consider using [Boost].SML
class StateMachine{
public:
    // REVIEW (Mitch): Transition enum class seems unnecessary. Could be changed to an
    //  atomic bool member.
    // REVIEW (Mitch): Upon further review, Transition enum class is used internally to call
    //  execute_transition function, as such, keep enum but stop using as "currently transitioning"
    //  flag.
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

        // REVIEW (Mitch): Change to `is_transitioning() -> bool`
        Transition get_transition() const;

        bool Configure();
        bool Activate();
        bool Passivate();
        bool Terminate();

        // REVIEW (Mitch): Decompose to register function per transition
        void RegisterTransitionFunction(const Transition& transation, const std::function<void ()>& func);

        static const std::string& ToString(const Transition& transation);
        static const std::string& ToString(const State& transation);
    private:
        // REVIEW (Mitch): rename "transition_state" to clearer verb such as "execute_transition"
        bool transition_state(const Transition& transition);
        bool run_transition_function(const Transition& transition);

        std::mutex transition_mutex_;

        // REVIEW (Mitch): Use compile time construct (std::array) given size is known
        std::unordered_map<Transition, std::function<void ()>, EnumClassHash> transition_map_;
        
        std::mutex state_mutex_;
        std::atomic<State> state_{State::NOT_CONFIGURED};

        // REVIEW (Mitch): Keeping track of which transition we're currently performing is unused.
        //  Consider atomic<bool>
        std::atomic<Transition> transition_{Transition::NO_TRANSITION};
};


#endif //STATE_MACHINE_H