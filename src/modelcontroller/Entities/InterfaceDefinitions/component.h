#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"

class EntityFactory;
class Component : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    Component(EntityFactory& factory, bool is_temp_node);
    void DataAdded(Data* data);
};

#endif // COMPONENT_H
