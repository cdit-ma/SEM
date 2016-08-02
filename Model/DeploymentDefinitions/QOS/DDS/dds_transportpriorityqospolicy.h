#ifndef DDS_TRANSPORTPRIORITYQOSPOLICY_H
#define DDS_TRANSPORTPRIORITYQOSPOLICY_H
#include "../../../node.h"

class DDS_TransportPriorityQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_TransportPriorityQosPolicy();
    ~DDS_TransportPriorityQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_TRANSPORTPRIORITYQOSPOLICY_H


