#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"

class Component : public Node
{
    Q_OBJECT
public:
    Component();
    ~Component();

    bool canAdoptChild(Node* child);
};

#endif // COMPONENT_H
