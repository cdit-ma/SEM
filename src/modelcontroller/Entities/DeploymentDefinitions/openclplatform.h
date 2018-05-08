#ifndef OPENCLPLATFORM_H
#define OPENCLPLATFORM_H
#include "../node.h"

class EntityFactory;
class OpenCLPlatform : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    OpenCLPlatform(EntityFactory& factory, bool is_temp_node);
};

#endif // OPENCLPLATFORM_H
