#ifndef INTERFACEDEFINITIONC_H
#define INTERFACEDEFINITIONC_H
#include "../node.h"

class EntityFactory;
class InterfaceDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InterfaceDefinitions(EntityFactory* factory);
	InterfaceDefinitions();
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};

#endif // INTERFACEDEFINITIONC_H
