#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventportdelegate.h"

class EntityFactory;
class OutEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPortInstance(EntityFactory* factory);
	OutEventPortInstance();
public:
    bool canAdoptChild(Node*);
};

#endif // OUTEVENTPORTINSTANCE_H
