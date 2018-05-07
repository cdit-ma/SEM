#ifndef MODEL_H
#define MODEL_H

#include "node.h"
class EntityFactory;
class Model: public Node
{
    friend class EntityFactory;
    Q_OBJECT
protected:
	Model(EntityFactory* factory);
    Model();
    static Node* ConstructModel(EntityFactory* factory);
public:
    bool canAdoptChild(Node*);
};
#endif // MODEL_H
