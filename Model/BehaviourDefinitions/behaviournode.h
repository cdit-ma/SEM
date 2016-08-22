#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"

class BehaviourNode : public Node
{
    Q_OBJECT
public:
    BehaviourNode(NODE_KIND kind);

    void setWorkflowProducer(bool producer);
    void setWorkflowReciever(bool reciever);

    bool isWorkflowProducer() const;
    bool isWorkflowReciever() const;

    BehaviourNode* getProducerNode();
    QList<BehaviourNode*> getRecieverNodes();

    BehaviourNode* getParentBehaviourNode();
    BehaviourNode* getInitialProducer();

    bool isNodeInBehaviourChain(BehaviourNode* node);



    virtual bool canAdoptChild(Node* child);
    virtual bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);

private:
    bool _isReciever;
    bool _isProducer;

};

#endif // BEHAVIOURNODE_H
