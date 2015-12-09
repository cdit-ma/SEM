#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"

class ComponentInstance : public Node
{
    Q_OBJECT
public:
    ComponentInstance();
    ~ComponentInstance();

    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // COMPONENTINSTANCE_H
