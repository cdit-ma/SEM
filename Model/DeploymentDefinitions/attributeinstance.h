#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../node.h"

#include <Qstring>

class Attribute;
class AttributeImpl;

class AttributeInstance : public Node
{
    Q_OBJECT
public:
    AttributeInstance(QString name="");
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
