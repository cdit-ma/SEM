#ifndef DDS_GROUPDATAQOSPOLICY_H
#define DDS_GROUPDATAQOSPOLICY_H
#include "../../../node.h"

class DDS_GroupDataQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_GroupDataQosPolicy();
    ~DDS_GroupDataQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_GROUPDATAQOSPOLICY_H


