#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <condition_variable>
#include <future>
#include <mutex>
#include <set>
#include <thread>

// REVIEW (Mitch): From what I can tell, this class is used as a synchronised state-machine

// REVIEW (Mitch): This class makes thread based code in PeriodicPort and
//  SubscriberPort difficult to reason about. The function started asynchronously is
//  given a handle to thread manager, which manages the thread that function is running on. This
//  gives the thread control over itself? While also introducing a state machine into the mix with
//  a, difficult to comprehend, number of mutexes.

// REVIEW (Jackson): Resolution, remove this class. Move current functionality to usage sites
//  (should be able to use a condition variable to mimic this class).
class ThreadManager{
    public:

        // REVIEW (Mitch): Is none a valid state? Rename to unconfigured
        enum class State{NONE, CONFIGURED, ACTIVE, TERMINATED};
        enum class Transition{ACTIVATE, TERMINATE};
        
        ~ThreadManager();
        void SetThread(std::unique_ptr<std::thread> thread);
        void SetFuture(std::future<void> async_future);

        // REVIEW (Mitch): Potential for confusion with Activatable Activate/Passivate/Terminate
        bool Activate();
        bool Terminate();

        void SetTerminate();

        bool WaitForActivated();
        bool WaitForConfigured();
    public:
        // REVIEW (Mitch): I think these were meant to be for internal use only, hence the "maybe
        //  should add access control". These functions have since leaked into public use
        //TODO: Maybe should add access control
        void Thread_Configured();
        void Thread_Terminated();
        void Thread_Activated();
        // REVIEW (Mitch): What's the difference between Thread_WaitForActivate and
        //  WaitForActivated?
        bool Thread_WaitForActivate();
        void Thread_WaitForTerminate();
    private:
        void SetState(State state);
        bool SetTransition(Transition transition);
        bool WaitForState(State state);
        bool WaitForTransition(Transition transition);

        std::unique_ptr<std::thread> thread_;
        std::future<void> future_;

        std::mutex state_mutex_;
        std::condition_variable state_condition_;
        
        std::set<State> states_;
        std::set<Transition> transitions_;

        std::mutex transition_mutex_;
        std::condition_variable transition_condition_;
};

#endif // THREAD_MANAGER_H