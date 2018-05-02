#ifndef RETURNPARAMETER_H
#define RETURNPARAMETER_H
#include "parameter.h"

class EntityFactory;
class ReturnParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ReturnParameter(EntityFactory* factory);
	ReturnParameter();

};

#endif // RETURNPARAMETER_H
