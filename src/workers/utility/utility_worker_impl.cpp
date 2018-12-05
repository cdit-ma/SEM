#include "utility_worker_impl.h"

#include <iostream>
#include <chrono>
#include <cmath>      // Math. functions needed for whets.cpp?
#include <stdarg.h>
#include <cstring>
#include "exprtk.hpp"
#include <thread>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string Utility_Worker_Impl::TimeOfDayString(){
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return std::to_string(time.count() / 1000.0);
}

double Utility_Worker_Impl::TimeOfDay(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}

double Utility_Worker_Impl::EvaluateComplexity(const char* complexity, va_list args){

    //Pull variables from expression
    std::vector<char> variable_list = ProcessVarList(complexity);

    //construct symbol table
    exprtk::symbol_table<double> symbol_table;

    return ExprtkWrapper::EvaluateComplexity(function, variable_map);
}

std::vector<std::string> Utility_Worker_Impl::ProcessVarList(const std::string& function){

    std::string stripped_function = function;

    static std::vector<std::string> symbols = {"1","2","3","4","5","6","7","8","9","0","*","/","+","-","(",")","^"};
    static std::vector<std::string> functions = {"abs", "acos", "asin", "atan", "atan2", "cos", "cosh",
                                                 "exp", "log", "log10", "sin", "sinh", "sqrt", "tan", "tanh",
                                                 "floor", "round", "min", "max", "sig", "log2", "epsilon", "pi", "infinity" };

    for(const auto& symbol : symbols) {
        ReplaceAllSubstring(stripped_function, symbol, " ");
    }

    for(const auto& func : functions) {
        ReplaceAllSubstring(stripped_function, func, " ");
    }


    //parse and compile symbol table and expression
    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);
    exprtk::parser<double> parser;
    parser.compile(complexity, expression);

    return expression.value();
}

std::vector<char> Utility_Worker_Impl::ProcessVarList(const char* complexity){
    char * cptr;
    char varlist[50];
    char mathchar[] = "()-+*/^0123456789";
    char mathfunc[24][9] = {"abs", "acos", "asin", "atan", "atan2", "cos", "cosh",
                            "exp", "log", "log10", "sin", "sinh", "sqrt", "tan", "tanh",
                            "floor", "round", "min", "max", "sig", "log2", "epsilon", "pi", "infinity" };
    //remove all math functions from complexity algorithm
    strcpy(varlist, complexity);

    for(unsigned int i = 0; i < 24; ++i){
        cptr = varlist;
        while ((cptr=strstr(cptr,mathfunc[i])) != NULL)
        memmove(cptr, cptr+strlen(mathfunc[i]), strlen(cptr+strlen(mathfunc[i]))+1);
    }
    
    //remove all math characters from varlist
    for(unsigned int i = 0; i < strlen(mathchar); ++i){
        cptr = varlist;
        while((cptr=strchr(cptr ,mathchar[i])) != NULL)
        memmove(cptr, cptr+1, strlen(cptr+1)+1);
    }
    //remove duplicate variables
    char * prev = varlist;
    while((*prev) != '\0'){
        cptr = prev + 1;
        while((cptr=strchr(cptr, (*prev))) != NULL){
            memmove(cptr, cptr+1, strlen(cptr+1)+1);
        }
        prev += 1;
    }
    std::string temp(varlist);
    std::vector<char> vec(temp.begin(), temp.end());
    return vec;
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