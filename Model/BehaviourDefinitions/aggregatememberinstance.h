#ifndef AGGREGATEMEMBERINSTANCE_H
#define AGGREGATEMEMBERINSTANCE_H
#include "../node.h"

class AggregateMemberInstance : public Node
{
public:
    AggregateMemberInstance();
    ~AggregateMemberInstance();

    // GraphML interface
    QString toString();
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};
#endif // AGGREGATEMEMBERINSTANCE_H
