#ifndef DATANODE_H
#define DATANODE_H
#include "../BehaviourDefinitions/behaviournode.h"

class DataNode : public BehaviourNode
{
public:
    DataNode(NODE_TYPE type);
    ~DataNode();
};

#endif // DATANODE_H
