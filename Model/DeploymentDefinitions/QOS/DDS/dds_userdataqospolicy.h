#ifndef DDS_USERDATAQOSPOLICY_H
#define DDS_USERDATAQOSPOLICY_H
#include "../../../node.h"

class DDS_UserDataQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_UserDataQosPolicy();
    ~DDS_UserDataQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_USERDATAQOSPOLICY_H


