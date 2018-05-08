#ifndef MODEL_IMPL_HEADER_H
#define MODEL_IMPL_HEADER_H

#include "../node.h"

class EntityFactory;
class Header: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    Header(EntityFactory& factory, bool is_temp_node);
};

#endif // MODEL_IMPL_HEADER_H
