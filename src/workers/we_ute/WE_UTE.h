#ifndef WE_UTE_H
#define WE_UTE_H

#include <string>
#include <core/worker.h>

class WE_UTE_Impl;

class WE_UTE : public Worker{
    public:
        WE_UTE(Component* component, std::string inst_name);
        ~WE_UTE();

        double EvaluateComplexity(const std::string complexity, ...);
        std::string TimeOfDayString();
        double TimeOfDay();
        void DebugMessage(const std::string format_str, ...);

    private:
        WE_UTE_Impl* impl_;
};

#endif  //WE_UTE_H
