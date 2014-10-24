#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

class AttributeInstance;
class AttributeImpl;


class Attribute : public Node
{
public:
    Attribute(QString name="");
    ~Attribute();

    void addInstance(AttributeInstance* instance);
    void removeInstance(AttributeInstance* instance);
    QVector<AttributeInstance*> getInstances();

    void setImpl(AttributeImpl* impl);
    AttributeImpl* getImpl();

    // GraphML interface
public:
    QString toString();
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
    QVector<AttributeInstance*> instances;
    AttributeImpl* impl;
};

#endif // ATTRIBUTE_H
