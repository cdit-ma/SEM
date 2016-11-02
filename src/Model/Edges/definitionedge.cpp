#include "definitionedge.h"
#include "../node.h"
DefinitionEdge::DefinitionEdge(Node *src, Node *dst) : Edge(src, dst, Edge::EC_DEFINITION)
{

}

DefinitionEdge *DefinitionEdge::createDefinitionEdge(Node *src, Node *dst)
{
    DefinitionEdge* edge = 0;
    if(src->canAcceptEdge(Edge::EC_DEFINITION, dst)){
        edge = new DefinitionEdge(src, dst);
    }
    return edge;
}


/**
 * @brief DefinitionEdge::isImplEdge Checks to see if the source is an Impl of the destination.
 * @return
 */
bool DefinitionEdge::isImplEdge() const
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
bool DefinitionEdge::isInstanceEdge() const
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
bool DefinitionEdge::isInterInstanceEdge() const
{
    if(isInstanceEdge()){
        Node* dst = getDestination();
        if(dst->isInstance()){
            return true;
        }
    }
    return false;
}
