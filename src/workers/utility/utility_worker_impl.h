#ifndef WORKERS_UTILITY_UTILITYWORKER_IMPL_H
#define WORKERS_UTILITY_UTILITYWORKER_IMPL_H

#include <mutex>
#include <string>
#include <vector>
#include <random>
#include <regex>
#include <boost/uuid/uuid_generators.hpp>

class Utility_Worker_Impl{
    public:
        double TimeOfDay();
        std::string TimeOfDayString();
        double EvaluateComplexity(const char* complexity, va_list args);

        void USleep(int microseconds);
        void Sleep(int seconds);

        void SetRandomSeed(unsigned int seed);
        int RandomUniformInt(int lower_bound, int upper_bound);
        double RandomUniformReal(double lower_bound = 0, double upper_bound = 1);
        double RandomNormalReal(double mean, double stddev);
        double RandomExponentialReal(double lambda);

        std::string GenerateUUID();
    private:
        std::vector<std::string> ProcessVarList(const std::string& function);
        void ReplaceAllSubstring(std::string& str, const std::string& search, const std::string& replace);


        std::default_random_engine random_generator_;
        boost::uuids::random_generator uuid_generator_;
};

#endif  //WORKERS_UTILITY_UTILITYWORKER_IMPL_H
