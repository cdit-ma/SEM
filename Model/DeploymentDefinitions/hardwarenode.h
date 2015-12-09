#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "hardware.h"

class HardwareNode : public Hardware
{
    Q_OBJECT
public:
    HardwareNode();
    ~HardwareNode();

    bool canAdoptChild(Node*);
};

#endif // HARDWARENODE_H
