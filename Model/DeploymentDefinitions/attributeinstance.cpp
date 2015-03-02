#include "attributeinstance.h"
#include "../InterfaceDefinitions/attribute.h"
#include "../DeploymentDefinitions/attributeinstance.h"
#include <QDebug>

AttributeInstance::AttributeInstance(QString name): Node(Node::NT_INSTANCE)
{
    //qDebug() << "Constructed Attribute: "<< this->getName();
}

AttributeInstance::~AttributeInstance()
{
    //Destructor
}


//An attribute cannot adopt anything.
bool AttributeInstance::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}


//An AttributeInstance can be connected to:
//Connected to a Definition:
// + Attribute
bool AttributeInstance::canConnect(Node* attachableObject)
{
    Attribute* attribute = dynamic_cast<Attribute*> (attachableObject);

    if(!attribute){
#ifdef DEBUG_MODE
        qWarning() << "AttributeInstance Node can only be connected to 1 Attribute Definition.";
#endif
        return false;
    }

    if(attribute && getDefinition()){
#ifdef DEBUG_MODE
        qWarning() << "AttributeInstance Node can only be connected to 1 Attribute Definition.";
#endif
        return false;
    }

    return Node::canConnect(attachableObject);
}

