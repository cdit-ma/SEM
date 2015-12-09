#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

class ComponentImpl: public Node
{
     Q_OBJECT
public:
    ComponentImpl();
    ~ComponentImpl();
    
public:
    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // COMPONENTBEHAVIOUR_H


