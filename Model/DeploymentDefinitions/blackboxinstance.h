#ifndef BLACKBOXINSTANCE_H
#define BLACKBOXINSTANCE_H
#include "../node.h"

class BlackBoxInstance: public Node
{
public:
    BlackBoxInstance();
    ~BlackBoxInstance();

public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);


};

#endif // BLACKBOXINSTANCE_H
