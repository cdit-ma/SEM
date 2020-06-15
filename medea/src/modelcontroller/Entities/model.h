#ifndef MODEL_H
#define MODEL_H

#include "node.h"

class EntityFactoryRegistryBroker;
class Model: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Model(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node*);
};
#endif // MODEL_H
