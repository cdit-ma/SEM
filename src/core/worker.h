#ifndef CORE_WORKER_H
#define CORE_WORKER_H

#include <mutex>
#include <string>
#include <stdarg.h>

#include "behaviourcontainer.h"
#include "modellogger.h"

class Worker: public BehaviourContainer{
    public:
        Worker(const BehaviourContainer& container, const std::string& class_name, const std::string& inst_name, bool is_worker = true);
        virtual ~Worker();

        const BehaviourContainer& get_container() const;
        
        std::string get_worker_name() const;
        
        std::string get_arg_string(const std::string str_format, va_list args);
        std::string get_arg_string_variadic(const std::string str_format, ...);

        bool is_custom_class() const;
        bool is_worker() const;

    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();

        int get_new_work_id();
        void Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");
        
    private:
        const BehaviourContainer& container_;
        const std::string worker_name_;
        const bool is_worker_class_;
        
        std::mutex mutex_;
        int work_id_ = 0;

};

#endif //CORE_WORKER_H
