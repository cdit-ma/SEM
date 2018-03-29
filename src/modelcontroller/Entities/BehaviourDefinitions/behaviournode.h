#ifndef BEHAVIOURNODE_H
#define BEHAVIOURNODE_H
#include "../node.h"

class EntityFactory;
class BehaviourNode : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    BehaviourNode(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    BehaviourNode(NODE_KIND kind);
public:

    void setWorkflowProducer(bool producer);
    void setWorkflowReciever(bool reciever);
    void setTopLevel(bool top_level);

    bool isWorkflowProducer() const;
    bool isWorkflowReciever() const;
    bool isTopLevel() const;

    BehaviourNode* getProducerNode();
    QList<BehaviourNode*> getRecieverNodes();

    BehaviourNode* getParentBehaviourNode();
    BehaviourNode* getInitialProducer();

    bool isNodeInBehaviourChain(BehaviourNode* node);



    virtual bool canAdoptChild(Node* child);
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    bool requiresEdgeKind(EDGE_KIND edgeKind);

private:
    bool _isReciever;
    bool _isProducer;
    bool is_top_level_;

};

#endif // BEHAVIOURNODE_H
