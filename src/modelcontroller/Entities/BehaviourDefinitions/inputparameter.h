#ifndef INPUTPARAMETER_H
#define INPUTPARAMETER_H
#include "parameter.h"

class EntityFactory;
class InputParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT
	
	protected:
		static void RegisterWithEntityFactory(EntityFactory& factory);
		InputParameter(EntityFactory& factory, bool is_temp_node);
};

#endif // INPUTPARAMETER_H
