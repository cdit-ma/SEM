#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "eventportassembly.h"

class EntityFactory;
class OutEventPortDelegate: public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPortDelegate(EntityFactory* factory);
	OutEventPortDelegate();
public:
    bool canAdoptChild(Node* child);
};


#endif // OUTEVENTPORTDELEGATE_H
