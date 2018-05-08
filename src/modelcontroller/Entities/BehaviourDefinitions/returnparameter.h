#ifndef RETURNPARAMETER_H
#define RETURNPARAMETER_H
#include "parameter.h"

class EntityFactory;
class ReturnParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
	ReturnParameter(EntityFactory& factory, bool is_temp_node);
};

#endif // RETURNPARAMETER_H
