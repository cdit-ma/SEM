#ifndef DDS_PARTITIONQOSPOLICY_H
#define DDS_PARTITIONQOSPOLICY_H
#include "../../../node.h"

class DDS_PartitionQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_PartitionQosPolicy();
    ~DDS_PartitionQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_PARTITIONQOSPOLICY_H


