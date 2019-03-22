#include "utility_worker_impl.h"

#include <iostream>
#include <chrono>
#include <cmath>      // Math. functions needed for whets.cpp?
#include <stdarg.h>
#include <cstring>
#include <thread>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <unordered_map>
#include <set>
#include <sstream>
#include "exprtkwrapper.h"

#include <core/logger.h>

std::string Utility_Worker_Impl::TimeOfDayString(){
    return std::to_string(TimeOfDay());
}

double Utility_Worker_Impl::TimeOfDay(){
    auto time = Logger::GetCurrentTime();
    //Second is what all other std::chrono::times ratios are based around
    double denominator = static_cast<double>(decltype(time)::period::den);
    return time.count() / denominator;
}

double Utility_Worker_Impl::EvaluateComplexity(const char* function, va_list args){

    std::unordered_map<std::string, double> variable_map;

    //Pull variables from expression
    std::vector<std::string> variable_list = ProcessVarList(function);

    for(const auto& var : variable_list){
        // Map variable to value in va_list
        double temp = va_arg(args, double);
        variable_map.insert({var, temp});
    }

    return ExprtkWrapper::EvaluateComplexity(function, variable_map);
}

std::vector<std::string> Utility_Worker_Impl::ProcessVarList(const std::string& function){

    std::string stripped_function = function;

    const static std::vector<std::string> symbols = {"1","2","3","4","5","6","7","8","9","0","*","/","+","-","(",")","^"};
    const static std::vector<std::string> functions = {"abs", "acos", "asin", "atan", "atan2", "cos", "cosh",
                                                 "exp", "log", "log10", "sin", "sinh", "sqrt", "tan", "tanh",
                                                 "floor", "round", "min", "max", "sig", "log2", "epsilon", "pi", "infinity" };

    for(const auto& symbol : symbols) {
        ReplaceAllSubstring(stripped_function, symbol, " ");
    }

    for(const auto& func : functions) {
        ReplaceAllSubstring(stripped_function, func, " ");
    }

    std::istringstream iss(stripped_function);
    std::vector<std::string> variable_names(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

    std::set<std::string> seen_names;

    auto iter = std::begin(variable_names);
    while(iter != std::end(variable_names)) {
        if(seen_names.find(*iter) != std::end(seen_names)) {
            iter = variable_names.erase(iter);
        } else {
            seen_names.insert(*iter);
            iter++;
        }
    }
    return variable_names;
}

void Utility_Worker_Impl::ReplaceAllSubstring(std::string& str, const std::string& search, const std::string& replace) {
    size_t position = 0;
    while((position = str.find(search, position)) != std::string::npos) {
        str.replace(position, search.length(), replace);
        position += replace.length();
    }
}

void Utility_Worker_Impl::USleep(int microseconds){
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}
void Utility_Worker_Impl::Sleep(int seconds){
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}


int Utility_Worker_Impl::RandomUniformInt(int lower_bound, int upper_bound){
    std::uniform_int_distribution<int> distribution(lower_bound, upper_bound);
    return distribution(random_generator_);
}

double Utility_Worker_Impl::RandomUniformReal(double lower_bound, double upper_bound){
    std::uniform_real_distribution<double> distribution(lower_bound, upper_bound);
    return distribution(random_generator_);
}

double Utility_Worker_Impl::RandomNormalReal(double mean, double stddev){
    std::normal_distribution<double> distribution(mean, stddev);
    return distribution(random_generator_);
}

double Utility_Worker_Impl::RandomExponentialReal(double lambda){
    std::exponential_distribution<double> distribution(lambda);
    return distribution(random_generator_);
}

std::string Utility_Worker_Impl::GenerateUUID(){
    return boost::uuids::to_string(uuid_generator_());
}


void Utility_Worker_Impl::SetRandomSeed(unsigned int seed){
    random_generator_.seed(seed);
}