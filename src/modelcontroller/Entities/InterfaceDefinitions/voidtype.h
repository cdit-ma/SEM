#ifndef VOIDTYPE_H
#define VOIDTYPE_H
#include "../node.h"

class EntityFactory;
class VoidType : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    VoidType(EntityFactory& factory, bool is_temp_node);
};

#endif // VOIDTYPE_H
