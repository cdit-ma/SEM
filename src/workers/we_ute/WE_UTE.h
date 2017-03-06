#ifndef WE_UTE_H
#define WE_UTE_H

#include <string>

class WE_UTE_Impl;

class WE_UTE{
    public:
        WE_UTE();
        ~WE_UTE();

        double EvaluateComplexity(const char* complexity, ...);
        std::string TimeOfDayString();
        double TimeOfDay();
        void DebugMessage(const char* fmtstr, ...);

    private:
        WE_UTE_Impl* impl_;
};

#endif  //WE_UTE_H
