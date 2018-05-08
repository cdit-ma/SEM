#ifndef VARIABLE_PARAMETER_H
#define VARIABLE_PARAMETER_H

#include "parameter.h"

class EntityFactory;
class VariableParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT

	protected:
		static void RegisterWithEntityFactory(EntityFactory& factory);
		VariableParameter(EntityFactory& factory, bool is_temp_node);
};

#endif // VARIABLE_PARAMETER_H
