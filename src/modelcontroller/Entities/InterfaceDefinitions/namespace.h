#ifndef NAMESPACE_H
#define NAMESPACE_H
#include "../node.h"

class EntityFactory;
class Namespace : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    Namespace(EntityFactory& factory, bool is_temp_node);
protected:
    void DataAdded(Data* data);
    void childAdded(Node* child);
    void childRemoved(Node* child);
    static void BindNamespace(Node* parent, Node* child, bool bind);
    void parentSet(Node* parent);
};

#endif // NAMESPACE_H
