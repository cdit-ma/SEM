#ifndef ATTRIBUTEIMPL_H
#define ATTRIBUTEIMPL_H
#include "../node.h"

class Attribute;

class AttributeImpl : public Node
{
public:
    AttributeImpl(QString name="");
    ~AttributeImpl();

    void setDefinition(Attribute* def);
    Attribute* getDefinition();


    // GraphML interface
public:
    QString toString();
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
private:
    Attribute* def;
};

#endif // ATTRIBUTEIMPL_H
