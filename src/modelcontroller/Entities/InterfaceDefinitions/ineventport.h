#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "eventport.h"

class EntityFactory;
class InEventPort : public EventPort
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPort(EntityFactory* factory);
	InEventPort();
};

#endif // INEVENTPORT_H
