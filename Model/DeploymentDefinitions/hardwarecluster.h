#ifndef HARDWARECLUSTER_H
#define HARDWARECLUSTER_H
#include "hardware.h"

class HardwareCluster : public Hardware
{
    Q_OBJECT
public:
    HardwareCluster();
    ~HardwareCluster();

    bool canAdoptChild(Node* child);
};

#endif // HARDWARECLUSTER_H
