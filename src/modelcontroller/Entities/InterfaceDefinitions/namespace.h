#ifndef NAMESPACE_H
#define NAMESPACE_H
#include "../node.h"

class EntityFactory;
class Namespace : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Namespace(EntityFactory* factory);
	Namespace();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);

protected:
    void DataAdded(Data* data);
    void childAdded(Node* child);
    void childRemoved(Node* child);
    static void BindNamespace(Node* parent, Node* child, bool bind);
    
};

#endif // NAMESPACE_H
