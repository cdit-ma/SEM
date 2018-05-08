#ifndef MODEL_H
#define MODEL_H

#include "node.h"
class EntityFactory;
class Model: public Node
{
    friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    Model(EntityFactory& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node*);
};
#endif // MODEL_H
