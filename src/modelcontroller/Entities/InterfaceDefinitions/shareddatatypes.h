#ifndef SHARED_DATATYPES_H
#define IDL_H
#include "../node.h"

class EntityFactory;
class SharedDatatypes : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	SharedDatatypes(EntityFactory* factory);
	SharedDatatypes();
};

#endif // IDL_H
