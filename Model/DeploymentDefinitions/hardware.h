#ifndef HARDWARE_H
#define HARDWARE_H
#include "../node.h"
class Hardware : public Node
{
    Q_OBJECT
public:
    Hardware(bool isHardwareNode);
    bool isHardwareNode();
    bool isHardwareCluster();

    bool canAdoptChild(Node* node);
private:
    bool hardwareNode;
};

#endif // HARDWARE_H
