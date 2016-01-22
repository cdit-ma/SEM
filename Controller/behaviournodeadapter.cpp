#include "behaviournodeadapter.h"
#include "nodeadapter.h"
BehaviourNodeAdapter::BehaviourNodeAdapter(BehaviourNode *node):NodeAdapter(node, NodeAdapter::NAK_BEHAVIOUR)
{

}

bool BehaviourNodeAdapter::hasDataOutput()
{
    if(isValid()){
        BehaviourNode* node = getBehaviourNode();
        if(node){
            if(node->isDataOutput()){
                return !node->gotDataOutput();
            }
        }
    }
    return false;
}

bool BehaviourNodeAdapter::hasDataInput()
{
    if(isValid()){
        BehaviourNode* node = getBehaviourNode();
        if(node){
            if(node->isDataInput()){
                return !node->gotDataInput();
            }
        }
    }
    return false;

}

bool BehaviourNodeAdapter::needsDataInput()
{
    if(isValid()){
        BehaviourNode* node = getBehaviourNode();
        if(node){
            if(node->isDataInput()){
                return !node->gotDataInput();
            }
        }
    }
    return false;
}

bool BehaviourNodeAdapter::needsDataOutput()
{
    if(isValid()){
        BehaviourNode* node = getBehaviourNode();
        if(node){
            return !node->gotDataOutput();
        }
    }
    return false;
}

bool BehaviourNodeAdapter::needsLeftEdge()
{
    if(isValid()){
        BehaviourNode* node = getBehaviourNode();
        if(node){
            return !node->gotLeftWorkflowEdge();
        }
    }
    return false;
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
