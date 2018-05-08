#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

class EntityFactory;
class Attribute : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    Attribute(EntityFactory& factory, bool is_temp_node);
};

#endif // ATTRIBUTE_H
