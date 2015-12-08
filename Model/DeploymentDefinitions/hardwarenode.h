#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"

class HardwareNode : public Node
{
    Q_OBJECT
public:
    HardwareNode();
    ~HardwareNode();
public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
    
};

#endif // HARDWARENODE_H
