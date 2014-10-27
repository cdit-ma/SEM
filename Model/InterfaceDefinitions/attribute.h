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

    // GraphML interface
public:
    QString toString();
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
};

#endif // ATTRIBUTE_H
