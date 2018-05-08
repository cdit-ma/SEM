#ifndef MODEL_IMPL_CODE_H
#define MODEL_IMPL_CODE_H

#include "../node.h"

class EntityFactory;
class Code: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    Code(EntityFactory& factory, bool is_temp_node);
};

#endif // MODEL_IMPL_CODE_H
