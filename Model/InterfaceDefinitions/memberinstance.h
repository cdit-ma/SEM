#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "../node.h"

class MemberInstance : public Node
{
public:
    MemberInstance();
    ~MemberInstance();

    // GraphML interface
    
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};


#endif // MEMBERINSTANCE_H
