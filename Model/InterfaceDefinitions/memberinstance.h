#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "../node.h"
#include "../BehaviourDefinitions/behaviournode.h"
class MemberInstance : public BehaviourNode
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
