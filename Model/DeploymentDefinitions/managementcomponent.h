#ifndef MANAGEMENTCOMPONENT_H
#define MANAGEMENTCOMPONENT_H

#include "../node.h"

class ManagementComponent : public Node
{
    Q_OBJECT
public:
    ManagementComponent();
    ~ManagementComponent();

    // Node interface
public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};

#endif // MANAGEMENTCOMPONENT_H
