#ifndef ENUM_H
#define ENUM_H
#include "../node.h"

class EntityFactory;
class Enum : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Enum(EntityFactory* factory);
	Enum();
    void DataAdded(Data* data);

    void childAdded(Node* child);
    void childRemoved(Node* child);
};

#endif //ENUM_H
