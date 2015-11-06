#include "parameter.h"
#include "behaviournode.h"
#include "../InterfaceDefinitions/memberinstance.h"
#include "variable.h"
#include "attributeimpl.h"


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
    bool connectOkay = false;
    if(memberInstance || variable || attributeImpl || parameter){
        bool canConnect = BehaviourNode::canConnect(node);
        //Ch
        if(canConnect){
            if(parameter && parameter->isInputParameter() == this->isInputParameter()){
                return false;
            }

            QString type = getDataValue("type");
            QStringList okayTypes;

            if(type == "vector"){
                //ANY VECTOR
                if(node->childrenCount() == 1){
                    Node* nodeChild = node->getChildren(0)[0];
                    if(nodeChild->getNodeKind() == "VectorInstance"){
                        connectOkay = true;
                    }
                }else{
                    connectOkay = false;
                }
            }else{
                if(type == "number"){
                    okayTypes << "int" << "float" << "double" << "longinteger";
                }
                QString connectType = node->getDataValue("type");
                if(okayTypes.contains(connectType.toLower())){
                    connectOkay = true;
                }
            }
        }

    }
    return connectOkay;
}

