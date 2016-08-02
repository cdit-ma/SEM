#ifndef DDS_DESTINATIONORDERQOSPOLICY_H
#define DDS_DESTINATIONORDERQOSPOLICY_H
#include "../../../node.h"

class DDS_DestinationOrderQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_DestinationOrderQosPolicy();
    ~DDS_DestinationOrderQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_DESTINATIONORDERQOSPOLICY_H


