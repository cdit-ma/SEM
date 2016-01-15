#include "behaviournodeadapter.h"
#include "nodeadapter.h"
BehaviourNodeAdapter::BehaviourNodeAdapter(BehaviourNode *node):NodeAdapter(node, NodeAdapter::NAK_BEHAVIOUR)
{

}

bool BehaviourNodeAdapter::needsConnection()
{
    if(isValid()){
        BehaviourNode* node = getBehaviourNode();
        if(node){
            return node->needEdge();
        }
    }
    return false;
}

BehaviourNode *BehaviourNodeAdapter::getBehaviourNode()
{
    if(isValid()){
        return (BehaviourNode*)getNode();
    }
    return 0;
}
