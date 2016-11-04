#ifndef DEFINITIONEDGE_H
#define DEFINITIONEDGE_H
#include "../edge.h"

class DefinitionEdge: public Edge{
    Q_OBJECT

protected:
    //Constructor
    DefinitionEdge(Node* src, Node* dst);

public:
    static DefinitionEdge* createDefinitionEdge(Node* src, Node* dst);

    bool isImplEdge() const;
    bool isInstanceEdge() const;
    bool isInterInstanceEdge() const;
};

#endif // EDGE_H
