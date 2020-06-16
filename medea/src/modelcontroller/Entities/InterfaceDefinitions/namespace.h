#ifndef NAMESPACE_H
#define NAMESPACE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class Namespace : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Namespace(EntityFactoryBroker& factory, bool is_temp_node);

protected:
    void DataAdded(Data* data);
    void childAdded(Node* child);
    void childRemoved(Node* child);
    static void BindNamespace(Node* parent, Node* child, bool bind);
    void parentSet(Node* parent);
};

#endif // NAMESPACE_H
