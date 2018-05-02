#ifndef MODEL_IMPL_CODE_H
#define MODEL_IMPL_CODE_H

#include "../node.h"

class EntityFactory;
class Code: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Code(EntityFactory* factory);
	Code();
};

#endif // MODEL_IMPL_CODE_H
