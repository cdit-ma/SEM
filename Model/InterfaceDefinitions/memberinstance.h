#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "../node.h"

class MemberInstance : public Node
{
    Q_OBJECT
public:
    MemberInstance();
    ~MemberInstance();

    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // MEMBERINSTANCE_H
