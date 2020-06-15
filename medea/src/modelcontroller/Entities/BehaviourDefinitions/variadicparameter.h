#ifndef VARIADICPARAMETER_H
#define VARIADICPARAMETER_H
#include "parameter.h"


class EntityFactoryRegistryBroker;
class VariadicParameter : public Parameter
{

    Q_OBJECT
	

	protected:
public:
		static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
		VariadicParameter(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // VARIADICPARAMETER_H
