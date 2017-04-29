#include "eventportdelegate.h"
#include <QDebug>
EventPortAssembly::EventPortAssembly(Node::NODE_KIND kind): EventPort(kind)
{
    setNodeType(NT_EVENTPORT_ASSEMBLY);
    setAcceptsEdgeKind(Edge::EC_ASSEMBLY);


    updateDefaultData("type", QVariant::String, true);

    if(kind == NK_INEVENTPORT_INSTANCE || kind == NK_OUTEVENTPORT_INSTANCE){
        updateDefaultData("topicName", QVariant::String);
        updateDefaultData("middleware", QVariant::String, false, "ZMQ");
    }
}

bool EventPortAssembly::isInPortDelegate() const
{
    return getNodeKind() == NK_INEVENTPORT_DELEGATE;
}

bool EventPortAssembly::isOutPortDelegate() const
{
    return getNodeKind() == NK_OUTEVENTPORT_DELEGATE;
}

bool EventPortAssembly::isInPortAssembly() const
{
    return isInPortDelegate() || isInPortInstance();
}

bool EventPortAssembly::isOutPortAssembly() const
{
    return isOutPortDelegate() || isOutPortInstance();

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

bool EventPortAssembly::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        qCritical() << "CANNOT ACCEPT EDGE TYPE: " << edgeKind;
        return false;
    }

    switch(edgeKind){
    case Edge::EC_ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(!dst->isNodeOfType(NT_EVENTPORT_ASSEMBLY)){
            return false;
        }

        //Can't have an assembly link without an aggregate.
        if(!getAggregate()){
            return false;
        }

        EventPortAssembly* port = (EventPortAssembly*) dst;

        //Can't connect different aggregates
        if(getAggregate() != port->getAggregate()){
            qCritical() << "Differeny aggregate Aggregate";
            return false;
        }

        int depthToAncestor = getDepthFromCommonAncestor(port);
        int depthToAncestorReverse = port->getDepthFromCommonAncestor(this);
        int difference = abs(depthToAncestor - depthToAncestorReverse);
        int totalDepth = depthToAncestor + depthToAncestorReverse;

        //Can connect in either the same Assembly or 1 different higher.
        if(difference > 1){
            qCritical() << "Different Depth";
            return false;
        }
        if(totalDepth > 4){
            return false;
        }

        if(difference == 0){
            //Different Parents
            if(depthToAncestor == 2){
                //Don't allow connections from the same type, inter assembly.
                if(isOutPortDelegate() && !port->isInPortDelegate()){
                    qCritical() << "Same Type of delegate";
                    return false;
                }
                if(isOutPortInstance() && !port->isInPortInstance()){
                    return false;
                }
            }else if(depthToAncestor == 1){
                if(isPortInstance() && port->isPortInstance()){
                    //Dont allow cycles into the same component
                    return false;
                }else if(isPortDelegate() && port->isPortDelegate()){
                    //No delegate to delegate.
                    return false;
                }else{
                    if(isInPortAssembly() != port->isInPortAssembly()){
                        return false;
                    }
                }
            }
        }else if(difference == 1){
            if(isInPortAssembly() != port->isInPortAssembly()){
                return false;
            }
        }

        if(isOutPortInstance()){
            if(!(port->isInPortInstance() || port->isPortDelegate())){
                return false;
            }
        }
        if(isOutPortDelegate()){
            if(!(port->isInPortInstance() || port->isPortDelegate())){
                return false;
            }
        }
        if(isInPortAssembly()){
            if(port->isInPortAssembly()){
                if(depthToAncestor >= depthToAncestorReverse){
                    return false;
                }
            }else{
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
