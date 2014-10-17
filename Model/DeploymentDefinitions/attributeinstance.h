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

    void setDefinition(Attribute* def);
    Attribute* getDefinition();

    AttributeImpl* getImpl();



    // GraphML interface
public:
    QString toString();
    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);

private:
    Attribute* def;
};

#endif // ATTRIBUTEINSTANCE_H
