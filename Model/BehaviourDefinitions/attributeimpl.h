#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "../node.h"

class AttributeImpl : public Node
{
public:
    AttributeImpl();
    ~AttributeImpl();

    // GraphML interface
public:
    
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // ATTRIBUTEIMPL_H
