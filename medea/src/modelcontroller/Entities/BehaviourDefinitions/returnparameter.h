#ifndef RETURNPARAMETER_H
#define RETURNPARAMETER_H
#include "parameter.h"


class EntityFactoryRegistryBroker;
class ReturnParameter : public Parameter
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	ReturnParameter(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // RETURNPARAMETER_H
