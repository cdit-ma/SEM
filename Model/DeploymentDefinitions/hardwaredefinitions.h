#ifndef HARDWAREDEFINITIONS_H
#define HARDWAREDEFINITIONS_H

#include "../node.h"

class HardwareDefinitions: public Node
{
    Q_OBJECT
public:
    HardwareDefinitions();
    ~HardwareDefinitions();


    // GraphML interface
public:
    

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};
#endif // HARDWAREDEFINITIONS_H
