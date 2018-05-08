#ifndef SHARED_DATATYPES_H
#define SHARED_DATATYPES_H
#include "../node.h"

class EntityFactory;
class SharedDatatypes : public Node
{
	friend class EntityFactory;
    Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        SharedDatatypes(EntityFactory& factory, bool is_temp_node);
};

#endif // SHARED_DATATYPES_H
