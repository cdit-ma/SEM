#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventportassembly.h"

class EntityFactory;
class InEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortInstance(EntityFactory* factory);
	InEventPortInstance();
public:
    bool canAdoptChild(Node*);
};

#endif // INEVENTPORTINSTANCE_H
