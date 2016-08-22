#include "eventportdelegate.h"

EventPortAssembly::EventPortAssembly(Node::NODE_KIND kind): EventPort(kind)
{
    setNodeType(NT_EVENTPORT_ASSEMBLY);
    setAcceptsEdgeKind(Edge::EC_ASSEMBLY);
}

bool EventPortAssembly::isInPortDelegate() const
{
    return getNodeKind() == NK_INEVENTPORT_DELEGATE;
}

bool EventPortAssembly::isOutPortDelegate() const
{
    return getNodeKind() == NK_OUTEVENTPORT_DELEGATE;
}

bool EventPortAssembly::isPortDelegate() const
{
    return isInPortDelegate() || isOutPortDelegate();
}

bool EventPortAssembly::isPortInstance() const
{
    return isInPortInstance() || isOutPortInstance();
}

bool EventPortAssembly::isInPortInstance() const
{
    return getNodeKind() == NK_INEVENTPORT_INSTANCE;
}

bool EventPortAssembly::isOutPortInstance() const
{
    return getNodeKind() == NK_OUTEVENTPORT_INSTANCE;
}

bool EventPortAssembly::canAdoptChild(Node* child)
{
    return EventPort::canAdoptChild(child);
}

bool EventPortAssembly::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(!dst->isNodeofType(NT_EVENTPORT_ASSEMBLY)){
            return false;
        }

        //Can't have an assembly link without an aggregate.
        if(!getAggregate()){
            return false;
        }

        EventPortAssembly* port = (EventPortAssembly*) dst;

        //Can't connect different aggregates
        if(getAggregate() != port->getAggregate()){
            return false;
        }

        int depthToAncestor = getHeightToCommonAncestor(port);
        int depthToAncestorReverse = port->getHeightToCommonAncestor(this);
        int difference = abs(depthToAncestor - depthToAncestorReverse);

        //Can connect in either the same Assembly or 1 different higher.
        if(difference > 1){
            return false;
        }

        //Inter Component Assembly Connections
        //if(isPortDelegate() && port->isPortDelegate()){
        if(difference == 0 && depthToAncestor == 2){
            //Don't allow connections from the same type, inter assembly.
            if(port->isInPortDelegate() == isInPortDelegate()){
                return false;
            }
            return false;
        }else if(difference == 1 && !(depthToAncestor > 2 || depthToAncestorReverse > 2)){
            //Inside an Assembly only allow Same to same.
            if(port->isInPortDelegate() != port->isInPortDelegate()){
                return false;
            }
        }
        break;
    }
    default:
        break;
    }
    return EventPort::canAcceptEdge(edgeKind, dst);
}
