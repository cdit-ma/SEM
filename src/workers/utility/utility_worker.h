#ifndef WORKERS_UTILITY_UTILITYWORKER_H
#define WORKERS_UTILITY_UTILITYWORKER_H

#include <string>
#include <core/worker.h>

class Utility_Worker_Impl;

class Utility_Worker: public Worker{
    public:
        Utility_Worker(Component* component, std::string inst_name);
        ~Utility_Worker();


        void Log(const std::string format_str, bool print, ...);
        double EvaluateComplexity(const std::string complexity, ...);
        
        std::string TimeOfDayString();
        double TimeOfDay();
    private:
        Utility_Worker_Impl* impl_ = 0;
};

#endif  //WORKERS_UTILITY_UTILITYWORKER_H
