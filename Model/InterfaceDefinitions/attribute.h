#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

class Attribute : public Node
{
    Q_OBJECT
public:
    Attribute();
    ~Attribute();

    bool canAdoptChild(Node* child);
};

#endif // ATTRIBUTE_H
