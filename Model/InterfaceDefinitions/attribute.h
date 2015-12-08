#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

class AttributeInstance;
class AttributeImpl;


class Attribute : public Node
{
public:
    Attribute();
    ~Attribute();

    // GraphML interface
public:
    
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
};

#endif // ATTRIBUTE_H
