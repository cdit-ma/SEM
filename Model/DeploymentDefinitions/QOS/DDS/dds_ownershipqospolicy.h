#ifndef DDS_OWNERSHIPQOSPOLICY_H
#define DDS_OWNERSHIPQOSPOLICY_H
#include "../../../node.h"

class DDS_OwnershipQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_OwnershipQosPolicy();
    ~DDS_OwnershipQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_OWNERSHIPQOSPOLICY_H


