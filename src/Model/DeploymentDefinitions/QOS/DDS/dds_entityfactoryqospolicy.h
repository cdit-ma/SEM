#ifndef DDS_ENTITYFACTORYQOSPOLICY_H
#define DDS_ENTITYFACTORYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_EntityFactoryQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_EntityFactoryQosPolicy(EntityFactory* factory);
	DDS_EntityFactoryQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_ENTITYFACTORYQOSPOLICY_H


