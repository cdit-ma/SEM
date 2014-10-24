#ifndef BLANKNODE_H
#define BLANKNODE_H
#include "node.h"
class BlankNode: public Node
{
        Q_OBJECT
public:
    BlankNode(QString name=0);
    ~BlankNode();

    // GraphML interface
    QString toString();

    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // BLANKNODE_H
