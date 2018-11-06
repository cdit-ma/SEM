#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <set>

class ThreadManager{
    public:
        enum class State{NONE, CONFIGURED, ACTIVE, TERMINATED};
        enum class Transition{ACTIVATE, TERMINATE};
        
        ~ThreadManager();
        void SetThread(std::unique_ptr<std::thread> thread);
        void SetFuture(std::future<void> async_future);
        
        bool Activate();
        bool Terminate();
        
        void SetTerminate();

        bool WaitForActivated();
        bool WaitForConfigured();
    public:
        //TODO: Maybe should add access control
        void Thread_Configured();
        void Thread_Terminated();
        void Thread_Activated();
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