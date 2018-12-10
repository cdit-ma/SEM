//
// Created by cdit-ma on 5/12/18.
//

#include "exprtkwrapper.h"
#include "exprtk.hpp"

double ExprtkWrapper::EvaluateComplexity(const std::string& function, std::unordered_map<std::string, double> variable_map){
    exprtk::symbol_table<double> symbol_table;

    for(const auto& var : variable_map){
        symbol_table.create_variable(var.first, var.second);
    }
    //Add infinity, pi and epsilon
    symbol_table.add_constants();

    //parse and compile symbol table and expression
    exprtk::expression<double> compiled_expression;
    compiled_expression.register_symbol_table(symbol_table);
    exprtk::parser<double> parser;
    parser.compile(function, compiled_expression);

    return compiled_expression.value();
}
