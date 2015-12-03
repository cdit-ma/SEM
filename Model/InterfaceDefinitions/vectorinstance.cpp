#include <QDebug>
#include "vectorinstance.h"
#include "aggregateinstance.h"
#include "memberinstance.h"
#include "vector.h"
#include "../BehaviourDefinitions/inputparameter.h"

VectorInstance::VectorInstance(): Node(Node::NT_DEFINSTANCE)
{

}

VectorInstance::~VectorInstance()
{

}

bool VectorInstance::canConnect(Node* attachableObject)
{
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(attachableObject);
    Vector* vector = dynamic_cast<Vector*>(attachableObject);
    InputParameter* inputParameter = dynamic_cast<InputParameter*>(attachableObject);


    if(!(vector || vectorInstance || inputParameter)){
#ifdef DEBUG_MODE
        qWarning() << "VectorInstance can only connect to an Vector.";
#endif
        return false;
    }

    if(getDefinition()){
        if(vector){
            #ifdef DEBUG_MODE
                qWarning() << "VectorInstance can only connect to one Vector.";
            #endif
            return false;
        }else if(vectorInstance){
            #ifdef DEBUG_MODE
                qWarning() << "VectorInstance can only connect to an VectorInstance which has a definition.";
            #endif
            return false;
        }
    }else{
        if(inputParameter){
            #ifdef DEBUG_MODE
                qWarning() << "VectorInstance can only be connected to an InputParameter if it has a Definition.";
            #endif
            return false;
        }
    }

    if(inputParameter){
        if(inputParameter->getDataValue("type") != "WE_UTE_Vector"){
            #ifdef DEBUG_MODE
                qWarning() << "VectorInstance can only be connected to an InputParameter which requries a WE_UTE_VECTOR.";
            #endif
            return false;
        }
    }


    Node* srcParent = this;
       while(srcParent){
           if(srcParent->getNodeKind().endsWith("Instance")){
               srcParent = srcParent->getParentNode();
           }else{
               break;
           }
       }

   Node* dstParent = attachableObject;
   while(dstParent){
       if(dstParent->getNodeKind().endsWith("Instance")){
           dstParent = dstParent->getParentNode();
       }else{
           break;
       }
   }

    //If both Vector and VectorInstance are in the same parent container (IDL) / Component Behaviour
    if(srcParent->getParentNode() == dstParent->getParentNode()){
        //Check if they are indirectly connected already (Aka check for cycles of VectorInstance.
        if(attachableObject->isIndirectlyConnected(srcParent)){
            qCritical() << "VectorInstance is already connected in directly to Node";
            return false;
        }
    }

    return Node::canConnect(attachableObject);
}

bool VectorInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!memberInstance && !aggregateInstance){
#ifdef DEBUG_MODE
        qWarning() << "VectorInstance can only adopt MemberInstance or AggregateInstance or VectorInstance";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
