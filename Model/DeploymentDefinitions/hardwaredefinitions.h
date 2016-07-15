#ifndef HARDWAREDEFINITIONS_H
#define HARDWAREDEFINITIONS_H

#include "../node.h"

class HardwareDefinitions: public Node
{
    Q_OBJECT
public:
    HardwareDefinitions();
    ~HardwareDefinitions();
    VIEW_ASPECT getViewAspect();

    bool canAdoptChild(Node* child);
};
#endif // HARDWAREDEFINITIONS_H
