#ifndef DDS_DEADLINEQOSPOLICY_H
#define DDS_DEADLINEQOSPOLICY_H
#include "../../../node.h"

class DDS_DeadlineQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_DeadlineQosPolicy();
    ~DDS_DeadlineQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_DEADLINEQOSPOLICY_H


