#include "attributeimpl.h"
#include "../InterfaceDefinitions/attribute.h"
#include <QDebug>

/**
 * @brief AttributeImpl::AttributeImpl Constructor
 * Calls Node::NT_IMPL as it is an Implementation of Attribute
 */
AttributeImpl::AttributeImpl():Node(Node::NT_IMPL)
{

}


AttributeImpl::~AttributeImpl()
{

}



bool AttributeImpl::canConnect(Node* attachableObject)
{
    Attribute* attribute = dynamic_cast<Attribute*>(attachableObject);

    if(getDefinition()){
         #ifdef DEBUG_MODE
        qWarning() << "AttributeImpl already has a definition already";
        #endif
        return false;
    }

    if(!attribute){
         #ifdef DEBUG_MODE
        qWarning() << "AttributeImpl can only connect to an Attribute.";
#endif
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool AttributeImpl::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}

