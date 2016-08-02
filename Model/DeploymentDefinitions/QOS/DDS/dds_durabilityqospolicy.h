#ifndef DDS_DURABILITYQOSPOLICY_H
#define DDS_DURABILITYQOSPOLICY_H
#include "../../../node.h"

class DDS_DurabilityQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_DurabilityQosPolicy();
    ~DDS_DurabilityQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_DURABILITYQOSPOLICY_H


