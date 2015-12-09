#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../node.h"

class AttributeInstance : public Node
{
    Q_OBJECT
public:
    AttributeInstance();
    ~AttributeInstance();

    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // ATTRIBUTEINSTANCE_H
