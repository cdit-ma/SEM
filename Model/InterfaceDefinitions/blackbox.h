#ifndef BLACKBOX_H
#define BLACKBOX_H
#include "../node.h"

class BlackBox : public Node
{
    Q_OBJECT
public:
    BlackBox();
    ~BlackBox();

    bool canAdoptChild(Node* child);
};

#endif // BLACKBOX_H
