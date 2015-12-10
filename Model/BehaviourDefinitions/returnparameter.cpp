#include "returnparameter.h"
#include "parameter.h"
#include "variable.h"

ReturnParameter:: ReturnParameter(): Parameter(false)
{
    setIsDataOutput(true);
}

ReturnParameter::~ReturnParameter()
{

}

