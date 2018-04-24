#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "eventportdelegate.h"

class EntityFactory;
class InEventPortDelegate: public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortDelegate(EntityFactory* factory);
	InEventPortDelegate();
public:
    bool canAdoptChild(Node* child);
};

#endif // INEVENTPORTDELEGATE_H
