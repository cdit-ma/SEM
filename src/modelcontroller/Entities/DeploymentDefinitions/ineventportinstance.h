#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventportassembly.h"

class EntityFactory;
class InEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    InEventPortInstance(EntityFactory& factory, bool is_temp_node);
private:
	void updateQOSEdge();
};

#endif // INEVENTPORTINSTANCE_H
