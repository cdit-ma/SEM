#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventportassembly.h"

class EntityFactory;
class OutEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    OutEventPortInstance(EntityFactory& factory, bool is_temp_node);
private:
	void updateQOSEdge();
};

#endif // OUTEVENTPORTINSTANCE_H
