#ifndef MANAGEMENTCOMPONENT_H
#define MANAGEMENTCOMPONENT_H
#include "../node.h"

class ManagementComponent : public Node
{
    Q_OBJECT
public:
    ManagementComponent();
    ~ManagementComponent();

    bool canAdoptChild(Node* child);
};

#endif // MANAGEMENTCOMPONENT_H
