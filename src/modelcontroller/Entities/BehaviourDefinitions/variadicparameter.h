#ifndef VARIADICPARAMETER_H
#define VARIADICPARAMETER_H
#include "parameter.h"

class EntityFactory;
class VariadicParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	VariadicParameter(EntityFactory* factory);
	VariadicParameter();
};

#endif // VARIADICPARAMETER_H
