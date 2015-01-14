#ifndef WORKLOAD_H
#define WORKLOAD_H

#include "../node.h"

class Workload: public Node
{
    Q_OBJECT
public:
    Workload();
    ~Workload();

    // GraphML interface
    

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // WORKLOAD_H
