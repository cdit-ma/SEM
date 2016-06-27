#ifndef DDS_ENTITYFACTORYQOSPOLICY_H
#define DDS_ENTITYFACTORYQOSPOLICY_H
#include "../../../node.h"

class DDS_EntityFactoryQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_EntityFactoryQosPolicy();
    ~DDS_EntityFactoryQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_ENTITYFACTORYQOSPOLICY_H


