#ifndef ENUM_H
#define ENUM_H
#include "../node.h"

class EntityFactory;
class Enum : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    Enum(EntityFactory& factory, bool is_temp_node);
    void DataAdded(Data* data);

    void childAdded(Node* child);
    void childRemoved(Node* child);
};

#endif //ENUM_H
