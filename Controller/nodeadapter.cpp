#include "nodeadapter.h"

NodeAdapter::NodeAdapter(Node *node): EntityAdapter(node)
{
    _node = node;
    _nodeClass = node->getNodeClass();
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
