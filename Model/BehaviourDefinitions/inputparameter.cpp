#include "inputparameter.h"

InputParameter::InputParameter(): Parameter(true)
{
    setIsDataInput(true);
}

InputParameter::~InputParameter()
{
}
