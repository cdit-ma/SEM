#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"

class EntityFactory;
class Component : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Component(EntityFactory* factory);
	Component();
    void DataAdded(Data* data);
};

#endif // COMPONENT_H
