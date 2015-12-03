#include "returnparameter.h"
#include "parameter.h"
#include "variable.h"

ReturnParameter::ReturnParameter(): Parameter(false)
{
}

bool ReturnParameter::canAdoptChild(Node *node)
{
    return false;
}

bool ReturnParameter::canConnect(Node *node)
{
    Variable* variable = dynamic_cast<Variable*>(node);
    if(variable){
        if(!compareData(variable, "type")){
            //Cannot connect to a variable which doesn't match type!
            return false;
        }
        return true;
    }


    return Parameter::canConnect(node);
}

