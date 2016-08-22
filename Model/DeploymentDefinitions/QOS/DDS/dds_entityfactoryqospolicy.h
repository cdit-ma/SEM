#ifndef DDS_ENTITYFACTORYQOSPOLICY_H
#define DDS_ENTITYFACTORYQOSPOLICY_H
#include "../../../node.h"

class DDS_EntityFactoryQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_EntityFactoryQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);

};
#endif // DDS_ENTITYFACTORYQOSPOLICY_H


