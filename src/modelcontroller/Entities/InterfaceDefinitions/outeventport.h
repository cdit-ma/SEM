#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "eventport.h"

class EntityFactory;
class OutEventPort : public EventPort
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPort(EntityFactory* factory);
	OutEventPort();
};

#endif // OUTEVENTPORT_H
