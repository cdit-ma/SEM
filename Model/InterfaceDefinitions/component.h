#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"

class ComponentInstance;
class ComponentImpl;

class Component : public Node
{
public:
    Component();
    ~Component();

    

    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);


};

#endif // COMPONENT_H
