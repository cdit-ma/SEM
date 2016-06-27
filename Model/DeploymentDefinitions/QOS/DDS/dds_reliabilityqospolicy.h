#ifndef DDS_RELIABILITYQOSPOLICY_H
#define DDS_RELIABILITYQOSPOLICY_H
#include "../../../node.h"

class DDS_ReliabilityQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_ReliabilityQosPolicy();
    ~DDS_ReliabilityQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_RELIABILITYQOSPOLICY_H


