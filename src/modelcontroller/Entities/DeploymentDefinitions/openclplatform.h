#ifndef OPENCLPLATFORM_H
#define OPENCLPLATFORM_H
#include "../node.h"

class EntityFactory;
class OpenCLPlatform : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OpenCLPlatform(EntityFactory* factory);
	OpenCLPlatform();
};

#endif // OPENCLPLATFORM_H
