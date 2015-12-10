#ifndef ASSEMBLYEDGE_H
#define ASSEMBLYEDGE_H
#include "../edge.h"

class AssemblyEdge : public Edge
{
public:
    AssemblyEdge(Node *src, Node *dst);
    ~AssemblyEdge();
    QString toString();
};
#endif // ASSEMBLYEDGE_H
