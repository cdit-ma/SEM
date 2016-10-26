#ifndef DEFINITIONEDGE_H
#define DEFINITIONEDGE_H
#include "../edge.h"

class DefinitionEdge: public Edge{
    Q_OBJECT
public:
    //Constructor
    DefinitionEdge(Node* src, Node* dst);
    ~DefinitionEdge();

    bool isImplEdge();
    bool isInstanceEdge();
    bool isInterInstanceEdge();

    QString toString();
};

#endif // EDGE_H
