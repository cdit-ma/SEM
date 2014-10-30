#ifndef PROCESS_H
#define PROCESS_H
#include "../node.h"

class Process: public Node
{
    Q_OBJECT
public:
    Process();
    ~Process();

    // GraphML interface
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};


#endif // PROCESS_H
