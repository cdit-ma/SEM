#include "aggregate.h"

Aggregate::Aggregate(): Node(Node::NK_AGGREGATE)
{
    setNodeType(Node::NT_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_AGGREGATE);
}

bool Aggregate::canAdoptChild(Node *child)
{
    if(!child){
        return false;
    }

    Node::NODE_KIND kind = child->getNodeKind();

    switch(kind){
    case NK_AGGREGATE_INSTANCE:
        break;
    case NK_MEMBER:
        break;
    case NK_VECTOR_INSTANCE:{
        Node* vector = child->getDefinition();
        if(vector && vector->hasChildren()){
            //Check first child of vector.
            Node* vectorChild = vector->getChildren(0)[0];

            //If first child has a definition.
            if(vectorChild && vectorChild->getDefinition()){
               Node* aggregate = vectorChild->getDefinition();
               if(this == aggregate || isAncestorOf(aggregate)){
                   return false;
               }
            }
        }
        break;
    }
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool Aggregate::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
