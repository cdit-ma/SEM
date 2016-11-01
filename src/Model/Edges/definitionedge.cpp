#include "definitionedge.h"
#include "../node.h"
DefinitionEdge::DefinitionEdge(Node *src, Node *dst) : Edge(src, dst, Edge::EC_DEFINITION)
{}

DefinitionEdge::~DefinitionEdge()
{
}

/**
 * @brief DefinitionEdge::isImplEdge Checks to see if the source is an Impl of the destination.
 * @return
 */
bool DefinitionEdge::isImplEdge()
{
    Node* src = getSource();
    Node* dst = getDestination();
    if(src && dst){
        if(src->isImpl()){
            if(src->getDefinition() == dst){
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief DefinitionEdge::isInstanceEdge Checks to see if the source is an Instance of the destination
 * @return
 */
bool DefinitionEdge::isInstanceEdge()
{
    Node* src = getSource();
    Node* dst = getDestination();
    if(src && dst){
        if(src->isInstance()){
            if(src->getDefinition() == dst){
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief DefinitionEdge::isInterInstanceEdge Checks to see if the source is an instance of the destination, and that the destination itself is also an instance.
 * @return
 */
bool DefinitionEdge::isInterInstanceEdge()
{
    if(isInstanceEdge()){
        Node* dst = getDestination();
        if(dst->isInstance()){
            return true;
        }
    }
    return false;
}

QString DefinitionEdge::toString()
{
    return QString("DefinitionEdge[%1]: [" + getSource()->toString() +"] <-> [" + getDestination()->toString() + "]").arg(this->getID());
}
