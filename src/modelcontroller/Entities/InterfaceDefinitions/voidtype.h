#ifndef VOIDTYPE_H
#define VOIDTYPE_H
#include "../node.h"

class EntityFactory;
class VoidType : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	VoidType(EntityFactory* factory);
	VoidType();
};

#endif // VOIDTYPE_H
