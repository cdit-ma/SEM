#include "assemblyedge.h"
#include "../node.h"

AssemblyEdge::AssemblyEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_ASSEMBLY)
{
}

AssemblyEdge::~AssemblyEdge()
{

}

QString AssemblyEdge::toString()
{
    return QString("AssemblyEdge[%1]: [" + getSource()->toString() +"] <-> [" + getDestination()->toString() + "]").arg(this->getID());
}

