#ifndef BLACKBOX_H
#define BLACKBOX_H
#include "../node.h"


class BlackBox : public Node
{
public:
    BlackBox();
    ~BlackBox();

    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};

#endif // BLACKBOX_H
