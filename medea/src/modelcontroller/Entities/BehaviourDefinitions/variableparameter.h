#ifndef VARIABLE_PARAMETER_H
#define VARIABLE_PARAMETER_H

#include "parameter.h"


class EntityFactoryRegistryBroker;
class VariableParameter : public Parameter
{

    Q_OBJECT


	protected:
public:
		static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
		VariableParameter(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // VARIABLE_PARAMETER_H
