#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../node.h"

#include <QString>

class Attribute;
class AttributeImpl;

class AttributeInstance : public Node
{
    Q_OBJECT
public:
    AttributeInstance();
    ~AttributeInstance();

    // GraphML interface
public:
    
    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
    Attribute* def;
};

#endif // ATTRIBUTEINSTANCE_H
