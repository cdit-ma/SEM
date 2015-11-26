#include "variable.h"
#include <QDebug>
#include "../InterfaceDefinitions/aggregateinstance.h"
#include "../InterfaceDefinitions/member.h"
#include "../InterfaceDefinitions/aggregate.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include "inputparameter.h"

Variable::Variable():BehaviourNode()
{

}

Variable::~Variable()
{
}

bool Variable::canConnect(Node* node)
{
    InputParameter* inputParameter = dynamic_cast<InputParameter*>(node);

    if(!inputParameter){
        return false;
    }
    if(inputParameter->hasEdges()){
        return false;
    }

    //Check what type child is.
    bool isVector = false;
    bool isAggregate = false;
    bool isPrimative = false;
    Node* firstChild = 0;
    if(hasChildren()){
        foreach(Node* child, getChildren(0)){
            AggregateInstance* agChild = dynamic_cast<AggregateInstance*>(child);
            VectorInstance* veChild = dynamic_cast<VectorInstance*>(child);
            firstChild = child;
            if(agChild){
                isAggregate = true;
            }else if(veChild){
                isVector = true;
            }
        }
    }else{
        isPrimative = true;
    }

    if(isVector){
        if(inputParameter->getDataValue("type") != "WE_UTE_Vector"){
            return false;
        }
    }else if(isAggregate){
        if(!(firstChild && (firstChild->getDataValue("type") == inputParameter->getDataValue("type")))){
            return false;
        }
    }else if(isPrimative){
        if(inputParameter->getDataValue("type") != getDataValue("type")){
            return false;
        }
    }else{
        return false;
    }

    return Node::canConnect(node);
}

bool Variable::canAdoptChild(Node* child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);

    if(!aggregateInstance && !vectorInstance){
#ifdef DEBUG_MODE
        qWarning() << "Aggregate can only adopt Member/Instances";
#endif
        return false;
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
