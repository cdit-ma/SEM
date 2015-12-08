#include "parameter.h"
#include "behaviournode.h"
#include "../InterfaceDefinitions/memberinstance.h"
#include "variable.h"
#include "attributeimpl.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include <QDebug>

Parameter::Parameter(bool isInput):BehaviourNode()
{
    inputParameter = isInput;
}

bool Parameter::isInputParameter()
{
    return inputParameter;
}

bool Parameter::isReturnParameter()
{
    return !inputParameter;
}

bool Parameter::hasConnection()
{
    return hasEdges();
}

bool Parameter::compareableTypes(Node *node)
{
    QStringList numberTypes;
    numberTypes << "ShortInteger" << "LongInteger" << "LongLongInteger";
    numberTypes << "UnsignedShortInteger" << "UnsignedLongInteger" << "UnsignedLongLongInteger";
    numberTypes << "FloatNumber" << "DoubleNumber" << "LongDoubleNumber";
    numberTypes << "Boolean" << "Byte";

    QStringList stringTypes;
    stringTypes << "String" << "WideString";

    if(node){
        //Types
        QString type1 = getDataValue("type");
        QString type2 = node->getDataValue("type");

        if(numberTypes.contains(type1) && numberTypes.contains(type2)){
            return true;
        }
        if(stringTypes.contains(type1) && stringTypes.contains(type2)){
            return true;
        }
    }
    return false;
}

bool Parameter::canAdoptChild(Node *node)
{
    return BehaviourNode::canAdoptChild(node);
}

bool Parameter::canConnect(Node *node)
{

    //Check for matching Members, Attributes variables and parameters.
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(node);
    Variable* variable = dynamic_cast<Variable*>(node);
    AttributeImpl* attributeImpl = dynamic_cast<AttributeImpl*>(node);
    Parameter* parameter = dynamic_cast<Parameter*>(node);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(node);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(node);
    bool connectOkay = false;

    if(memberInstance || variable || attributeImpl || parameter || vectorInstance || aggregateInstance){
        int thisImplDepth = getDepthToAspect() - 1;
        int otherImplDepth = node->getDepthToAspect() - 1;

        if(thisImplDepth >=0 && otherImplDepth >= 0){
            if(getParentNode(thisImplDepth) != node->getParentNode(otherImplDepth)){
                return false
            }
        }else{
            return false;
        }


        bool canConnect = Node::canConnect(node);
        //Ch
        if(canConnect){
            if(hasEdges()){
                return false;
            }
            if(parameter && parameter->isInputParameter() == isInputParameter()){
                return false;
            }

            QString parameterType = getDataValue("type");
            QString parentNodeKind = node->getParentNode()->getNodeKind();

            if(node->isInstance()){
                //Step up until a non-instance.
                Node* parentNode = node->getParentNode();
                while(parentNode){
                    if(!parentNode->isInstance()){
                        break;
                    }else{
                        parentNode = parentNode->getParentNode();
                    }
                }
                if(parentNode){
                    parentNodeKind = parentNode->getNodeKind();
                }
            }

            bool matchedTypes = false;
            if(parameterType == "WE_UTE_Vector"){
                if(vectorInstance){
                    matchedTypes = true;
                }
            }else{
                if(compareData(node, "type")){
                    matchedTypes = true;
                }else if(compareableTypes(node)){
                    matchedTypes = true;
                }
            }

            if(matchedTypes){
                if(isReturnParameter()){
                    QStringList validParentTypes;
                    validParentTypes << "Variable" << "OutEventPortImpl";

                    if(!(validParentTypes.contains(parentNodeKind) || validParentTypes.contains(node->getNodeKind()))){
                        return false;
                    }
                }
                connectOkay = true;
            }
        }
    }else{
        return false;
    }
    return connectOkay;
}

