#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

#include <QString>

class Component;
class ComponentImpl: public Node
{
     Q_OBJECT
public:
    ComponentImpl();
    ~ComponentImpl();    // GraphML interface
    

public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};

#endif // COMPONENTBEHAVIOUR_H


