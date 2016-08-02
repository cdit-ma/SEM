#ifndef DDS_TIMEBASEDFILTERQOSPOLICY_H
#define DDS_TIMEBASEDFILTERQOSPOLICY_H
#include "../../../node.h"

class DDS_TimeBasedFilterQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_TimeBasedFilterQosPolicy();
    ~DDS_TimeBasedFilterQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_TIMEBASEDFILTERQOSPOLICY_H


