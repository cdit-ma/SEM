#include "eventportdelegate.h"

#include "../../edgekinds.h"
#include <QDebug>

EventPortAssembly::EventPortAssembly(EntityFactory* factory, NODE_KIND kind, QString kind_str) : EventPort(factory, kind, kind_str){
    RegisterDefaultData(factory, kind, "type", QVariant::String, true);

    if(kind == NODE_KIND::INEVENTPORT_INSTANCE || kind == NODE_KIND::OUTEVENTPORT_INSTANCE){
        
        QList<QVariant> values;
        values << "ZMQ";
        values << "RTI";
        values << "OSPL";
        values << "QPID";
        
        RegisterValidDataValues(factory, kind, "middleware", QVariant::String, values);

        RegisterDefaultData(factory, kind, "topicName", QVariant::String, false);
        RegisterDefaultData(factory, kind, "middleware", QVariant::String, false, values.first());
    }
};

EventPortAssembly::EventPortAssembly(NODE_KIND kind): EventPort(kind)
{
    setNodeType(NODE_TYPE::EVENTPORT_ASSEMBLY);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY);
}

bool EventPortAssembly::isInPortDelegate() const
{
    return getNodeKind() == NODE_KIND::INEVENTPORT_DELEGATE;
}

bool EventPortAssembly::isOutPortDelegate() const
{
    return getNodeKind() == NODE_KIND::OUTEVENTPORT_DELEGATE;
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
    return getNodeKind() == NODE_KIND::INEVENTPORT_INSTANCE;
}

bool EventPortAssembly::isOutPortInstance() const
{
    return getNodeKind() == NODE_KIND::OUTEVENTPORT_INSTANCE;
}

bool EventPortAssembly::canAdoptChild(Node* child)
{
    return EventPort::canAdoptChild(child);
}

bool EventPortAssembly::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(!dst->isNodeOfType(NODE_TYPE::EVENTPORT_ASSEMBLY)){
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

        int depthToAncestor = getDepthFromCommonAncestor(port);
        int depthToAncestorReverse = port->getDepthFromCommonAncestor(this);
        int difference = abs(depthToAncestor - depthToAncestorReverse);
        int totalDepth = depthToAncestor + depthToAncestorReverse;

        //Can connect in either the same Assembly or 1 different higher.
        if(difference > 1){
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
