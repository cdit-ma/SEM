#ifndef WORKERS_UTILITY_UTILITYWORKER_IMPL_H
#define WORKERS_UTILITY_UTILITYWORKER_IMPL_H

#include <mutex>
#include <string>
#include <vector>

class UtilityWorker_Impl{
    public:
        double TimeOfDay();
        std::string TimeOfDayString();
        double EvaluateComplexity(const char* complexity, va_list args);
    
    private:
        std::vector<char> ProcessVarList(const char* complexity);
        std::mutex lock_;
};

#endif  //WORKERS_UTILITY_UTILITYWORKER_IMPL_H
