//
// Created by cdit-ma on 5/12/18.
//

#ifndef PROJECT_EXPRTKWRAPPER_H
#define PROJECT_EXPRTKWRAPPER_H

#include <string>
#include <unordered_map>

namespace ExprtkWrapper{
    double EvaluateComplexity(const std::string& expression, std::unordered_map<std::string, double>);
}

#endif //PROJECT_EXPRTKWRAPPER_H
