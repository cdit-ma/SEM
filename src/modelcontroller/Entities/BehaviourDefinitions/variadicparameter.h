#ifndef VARIADICPARAMETER_H
#define VARIADICPARAMETER_H
#include "parameter.h"

class EntityFactory;
class VariadicParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT
	
	protected:
		static void RegisterWithEntityFactory(EntityFactory& factory);
		InputParameter(EntityFactory& factory, bool is_temp_node);
};

#endif // VARIADICPARAMETER_H
