#include "nodeadapter.h"
#include <QDebug>
NodeAdapter::NodeAdapter(Node *node, NodeAdapter::NODE_ADAPTER_KIND nodeAdapterKind):EntityAdapter(node)
{
    _node = node;
    _nodeClass = node->getNodeClass();
    _nodeAdapterKind = nodeAdapterKind;
    connect(node, SIGNAL(node_EdgeAdded(int,Edge::EDGE_CLASS)), this, SIGNAL(edgeAdded(int, Edge::EDGE_CLASS)));
    connect(node, SIGNAL(node_EdgeRemoved(int,Edge::EDGE_CLASS)), this, SIGNAL(edgeRemoved(int,Edge::EDGE_CLASS)));
    connect(node, SIGNAL(node_GotDefinition(bool)), this, SIGNAL(gotDefinition(bool)));
}



bool NodeAdapter::isDefinition()
{
    if(isValid()){
        return _node->isDefinition();
    }
    return false;
}

bool NodeAdapter::isInstance()
{
    if(isValid()){
        return _node->isInstance();
    }
    return false;
}

bool NodeAdapter::isAspect()
{
    if(isValid()){
        return _node->isAspect();
    }
    return false;

}

bool NodeAdapter::isImpl()
{
    if(isValid()){
        return _node->isImpl();
    }
    return false;
}

bool NodeAdapter::isHardware()
{
    if(isValid()){
        return _node->isHardware();
    }
    return false;
}

bool NodeAdapter::gotDefinition()
{
    if(isValid()){
        return _node->getDefinition() != 0;
    }
    return false;
}

int NodeAdapter::getDefinitionID()
{
    if(isValid()){
        if(_node->getDefinition()){
            qCritical() << _node->getDefinition()->toString();
            return _node->getDefinition()->getID();
        }
    }
    return -1;

}

bool NodeAdapter::gotInstances()
{
    if(isValid()){
        return! _node->getInstances().isEmpty();
    }
    return false;

}

bool NodeAdapter::gotImpl()
{
    if(isValid()){
        return !_node->getImplementations().isEmpty();
    }
    return false;

}

int NodeAdapter::childCount()
{
    if(isValid()){
        return _node->childrenCount();
    }
    return -1;
}

int NodeAdapter::edgeCount()
{
    if(isValid()){
        return _node->edgeCount();
    }
    return -1;

}

bool NodeAdapter::isBehaviourAdapter()
{
    return _nodeAdapterKind == NAK_BEHAVIOUR;
}

QList<int> NodeAdapter::getEdgeIDs(Edge::EDGE_CLASS edgeClass)
{
    if(isValid()){
        return _node->getEdgeIDs(edgeClass);
    }
    return QList<int>();
}

QList<int> NodeAdapter::getTreeIndex()
{
    if(isValid()){
        return _node->getTreeIndex();
    }
    return QList<int>();
}

int NodeAdapter::getParentNodeID(int depth)
{
    if(isValid()){
        Node* node =_node->getParentNode(depth);
        if(node){
            return node->getID();
        }
    }
    return -1;
}


NODE_CLASS NodeAdapter::getNodeClass()
{
    return _nodeClass;
}

Node *NodeAdapter::getNode()
{
    if(isValid()){
        return _node;
    }
    return 0;
}
