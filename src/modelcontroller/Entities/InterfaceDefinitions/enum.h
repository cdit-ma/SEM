#ifndef ENUM_H
#define ENUM_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class Enum : public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Enum(EntityFactoryBroker& factory, bool is_temp_node);
    void DataAdded(Data* data);

    void childAdded(Node* child);
    void childRemoved(Node* child);
};

#endif //ENUM_H
