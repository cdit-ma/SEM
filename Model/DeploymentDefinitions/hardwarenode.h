#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"

class HardwareNode : public Node
{
    Q_OBJECT
public:
    HardwareNode(QString name="");
    ~HardwareNode();
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
    
};

#endif // HARDWARENODE_H
