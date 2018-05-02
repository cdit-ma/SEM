#ifndef MODEL_IMPL_HEADER_H
#define MODEL_IMPL_HEADER_H

#include "../node.h"

class EntityFactory;
class Header: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Header(EntityFactory* factory);
	Header();
};

#endif // MODEL_IMPL_HEADER_H
