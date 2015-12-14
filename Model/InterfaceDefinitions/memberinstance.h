#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "../node.h"
#include "datanode.h"
class MemberInstance : public DataNode
{
    Q_OBJECT
public:
    MemberInstance();
    ~MemberInstance();

    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
    bool canConnect_DataEdge(Node *node);
    bool canConnect_WorkflowEdge(Node *node);
};

#endif // MEMBERINSTANCE_H
