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

    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // BLANKNODE_H
