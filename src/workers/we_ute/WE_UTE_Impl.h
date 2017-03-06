#ifndef WE_UTE_IMPL_H
#define WE_UTE_IMPL_H

#include <mutex>
#include <string>
#include <vector>

class WE_UTE_Impl{
    public:
        double TimeOfDay();
        std::string TimeOfDayString();
        double EvaluateComplexity(const char* complexity, va_list args);
        void DebugMessage(const char* fmtstr, va_list args);
    
    private:
        std::vector<char> ProcessVarList(const char* complexity);
        std::mutex lock_;
};

#endif  //WE_UTE_IMPL_H
