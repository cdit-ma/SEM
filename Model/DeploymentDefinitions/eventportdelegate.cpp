#include "eventportdelegate.h"
#include <QDebug>
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
        qCritical() << "CANNOT ACCEPT EDGE TYPE: " << edgeKind;
        return false;
    }

    switch(edgeKind){
    case Edge::EC_ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(!dst->isNodeOfType(NT_EVENTPORT_ASSEMBLY)){
            qCritical() << "NOT EVeNTPORT Assembly";
            return false;
        }

        //Can't have an assembly link without an aggregate.
        if(!getAggregate()){
            qCritical() << "No Aggregate";
            return false;
        }

        EventPortAssembly* port = (EventPortAssembly*) dst;

        //Can't connect different aggregates
        if(getAggregate() != port->getAggregate()){
            qCritical() << "Differeny aggregate Aggregate";
            return false;
        }

        int depthToAncestor = getHeightToCommonAncestor(port);
        int depthToAncestorReverse = port->getHeightToCommonAncestor(this);
        int difference = abs(depthToAncestor - depthToAncestorReverse);

        //Can connect in either the same Assembly or 1 different higher.
        if(difference > 1){
            qCritical() << "Different Depth";
            return false;
        }

        //Inter Component Assembly Connections
        if(difference == 0 && depthToAncestor == 2){
            //Don't allow connections from the same type, inter assembly.
            if(isOutPortDelegate() && !port->isInPortDelegate()){
                qCritical() << "Same Type of delegate";
                return false;
            }
        }else if(difference == 1 && !(depthToAncestor > 2 || depthToAncestorReverse > 2)){
            //Inside an Assembly only allow Same to same.
            if(port->isInPortDelegate() != port->isInPortDelegate()){
                qCritical() << "Same Type of delegate2";
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
