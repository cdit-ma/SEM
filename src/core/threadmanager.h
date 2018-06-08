#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadManager{
    public:
        enum class State{NONE, CONFIGURED, ACTIVE, TERMINATED};
        enum class Transition{NONE, ACTIVATE, TERMINATE};
        
        ~ThreadManager();
        void SetThread(std::unique_ptr<std::thread> thread);
        bool Configure();
        bool Activate();
        bool Terminate();
        State GetState();
    public:
        //TODO: Maybe should add access control
        void Thread_Configured();
        void Thread_Terminated();
        void Thread_Activated();
        bool Thread_WaitForActivate();
        void Thread_WaitForTerminate();
    private:
        std::unique_ptr<std::thread> thread_;

        std::mutex state_mutex_;
        std::condition_variable state_condition_;
        State state_ = State::NONE;

        std::mutex transition_mutex_;
        std::condition_variable transition_condition_;
        Transition transition_ = Transition::NONE;
};

#endif // THREAD_MANAGER_H