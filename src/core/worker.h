#ifndef CORE_WORKER_H
#define CORE_WORKER_H

#include <mutex>
#include <string>

#include "component.h"
#include "activatable.h"

class Worker: public Activatable{
    public:
        Worker(Component* component, std::string worker_name, std::string inst_name);
        virtual ~Worker();

        Component* get_component();
        std::string get_worker_name();
    protected:
        int get_new_work_id();
        std::string get_arg_string(const std::string str_format, va_list args);
    private:
        Component* component_ = 0;
        std::mutex mutex_;
        std::string worker_name_;
        int work_id_ = 0;
};

#endif //CORE_WORKER_H