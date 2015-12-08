#ifndef BLACKBOXINSTANCE_H
#define BLACKBOXINSTANCE_H
#include "../node.h"

class BlackBoxInstance: public Node
{
    Q_OBJECT
public:
    BlackBoxInstance();
    ~BlackBoxInstance();

    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // BLACKBOXINSTANCE_H
