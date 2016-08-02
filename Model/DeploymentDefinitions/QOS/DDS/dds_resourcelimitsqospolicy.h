#ifndef DDS_RESOURCELIMITSQOSPOLICY_H
#define DDS_RESOURCELIMITSQOSPOLICY_H
#include "../../../node.h"

class DDS_ResourceLimitsQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_ResourceLimitsQosPolicy();
    ~DDS_ResourceLimitsQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_RESOURCELIMITSQOSPOLICY_H


