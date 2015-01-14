#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "../node.h"

class AttributeImpl : public Node
{
public:
    AttributeImpl(QString name="");
    ~AttributeImpl();

    // GraphML interface
public:
    
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // ATTRIBUTEIMPL_H
