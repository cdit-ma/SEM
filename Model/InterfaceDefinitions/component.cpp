#include "component.h"
#include "../DeploymentDefinitions/hardwarecluster.h"
#include "../DeploymentDefinitions/hardwarenode.h"
#include "outeventport.h"
#include "ineventport.h"
#include "attribute.h"
#include <QDebug>

Component::Component(): Node(Node::NT_DEFINITION)
{
}

Component::~Component()
{

}

Edge::EDGE_CLASS Component::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool Component::canAdoptChild(Node *child)
{

    OutEventPort* outEventPort  = dynamic_cast<OutEventPort*>(child);
    InEventPort* inEventPort  = dynamic_cast<InEventPort*>(child);
    Attribute* attribute  = dynamic_cast<Attribute*>(child);

    if(!outEventPort && !inEventPort && !attribute){
#ifdef DEBUG_MODE
        qWarning() << "Can only adopt an OutEventPort, an InEventPort or an Attribute in a Component.";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
