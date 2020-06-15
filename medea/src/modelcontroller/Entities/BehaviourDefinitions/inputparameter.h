#ifndef INPUTPARAMETER_H
#define INPUTPARAMETER_H
#include "parameter.h"


class EntityFactoryRegistryBroker;
class InputParameter : public Parameter
{

    Q_OBJECT
	

	protected:
public:
		static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
		InputParameter(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // INPUTPARAMETER_H
