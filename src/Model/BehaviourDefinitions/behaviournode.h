#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"

class EntityFactory;
class BehaviourNode : public Node
{
    Q_OBJECT
protected:
    BehaviourNode(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    BehaviourNode(NODE_KIND kind);
public:

    void setWorkflowProducer(bool producer);
    void setWorkflowReciever(bool reciever);

    bool isWorkflowProducer() const;
    bool isWorkflowReciever() const;

    BehaviourNode* getProducerNode() const;
    QList<BehaviourNode*> getRecieverNodes() const;

    BehaviourNode* getParentBehaviourNode();
    BehaviourNode* getInitialProducer();

    bool isNodeInBehaviourChain(BehaviourNode* node);



    virtual bool canAdoptChild(Node* child);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
    bool requiresEdgeKind(Edge::EDGE_KIND edgeKind) const;

private:
    bool _isReciever;
    bool _isProducer;

};

#endif // BEHAVIOURNODE_H
