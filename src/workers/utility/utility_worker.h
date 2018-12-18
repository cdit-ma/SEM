#ifndef WORKERS_UTILITY_UTILITYWORKER_H
#define WORKERS_UTILITY_UTILITYWORKER_H

#include <string>
#include <core/worker.h>

class Utility_Worker_Impl;
class Utility_Worker : public Worker{
    public:
        Utility_Worker(const BehaviourContainer& component, const std::string& inst_name);
        ~Utility_Worker();

        void Marker(const std::string& marker_name, int64_t mark_id);
        void Log(const std::string format_str, int log_level, ...);
        double EvaluateComplexity(const std::string complexity, ...);
        
        std::string GetTimeOfDayString();
        double GetTimeOfDay();

        void USleep(int microseconds);
        void Sleep(int seconds);

        void SetRandomSeed(unsigned int seed);
        int RandomUniformInt(int lower_bound, int upper_bound);
        double RandomUniformReal(double lower_bound = 0, double upper_bound = 1);
        double RandomNormalReal(double mean, double stddev);
        double RandomExponentialReal(double lambda);
        std::string GenerateUUID();
    private:
        std::unique_ptr<Utility_Worker_Impl> impl_;
};

#endif  //WORKERS_UTILITY_UTILITYWORKER_H
