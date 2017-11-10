#ifndef CORE_WORKER_H
#define CORE_WORKER_H

#include <mutex>
#include <string>
#include <stdarg.h>
#include "component.h"
#include "activatable.h"
#include "modellogger.h"

class Worker: public Activatable{
    public:
        Worker(std::shared_ptr<Component> component, std::string worker_name, std::string inst_name);
        virtual ~Worker();

        std::shared_ptr<Component> get_component();
        std::string get_worker_name();
        
        std::string get_arg_string(const std::string str_format, va_list args);
        std::string get_arg_string_variadic(const std::string str_format, ...);

    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
        int get_new_work_id();
        void Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");
        
    private:
        std::shared_ptr<Component> component_;
        std::mutex mutex_;
        std::string worker_name_;
        int work_id_ = 0;
};

#endif //CORE_WORKER_H
