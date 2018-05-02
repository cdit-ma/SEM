#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventportassembly.h"

class EntityFactory;
class OutEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPortInstance(EntityFactory* factory);
	OutEventPortInstance();
};

#endif // OUTEVENTPORTINSTANCE_H
