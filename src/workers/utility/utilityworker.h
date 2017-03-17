#ifndef WORKERS_UTILITY_UTILITYWORKER_H
#define WORKERS_UTILITY_UTILITYWORKER_H

#include <string>
#include <core/worker.h>

class UtilityWorker_Impl;

class UtilityWorker: public Worker{
    public:
        UtilityWorker(Component* component, std::string inst_name);
        ~UtilityWorker();


        void DebugMessage(const std::string format_str, ...);
        double EvaluateComplexity(const std::string complexity, ...);
        
        std::string TimeOfDayString();
        double TimeOfDay();
    private:
        UtilityWorker_Impl* impl_ = 0;
};

#endif  //WORKERS_UTILITY_UTILITYWORKER_H
