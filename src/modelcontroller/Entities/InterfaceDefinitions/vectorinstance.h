#ifndef VECTORINSTANCE_H
#define VECTORINSTANCE_H
#include "datanode.h"

class EntityFactory;
class VectorInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    VectorInstance(EntityFactory& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};


#endif // AGGREGATE_H

