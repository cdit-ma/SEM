#ifndef INPUTPARAMETER_H
#define INPUTPARAMETER_H
#include "parameter.h"

class EntityFactory;
class InputParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InputParameter(EntityFactory* factory);
	InputParameter();
};

#endif // INPUTPARAMETER_H
