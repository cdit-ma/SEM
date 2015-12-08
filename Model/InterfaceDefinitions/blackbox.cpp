#include "blackbox.h"
#include "outeventport.h"
#include "ineventport.h"
#include <QDebug>
BlackBox::BlackBox():Node(Node::NT_DEFINITION)
{
}

BlackBox::~BlackBox()
{

}

Edge::EDGE_CLASS BlackBoxcanConnect(Node *attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool BlackBox::canAdoptChild(Node *child)
{
    OutEventPort* outEventPort  = dynamic_cast<OutEventPort*>(child);
    InEventPort* inEventPort  = dynamic_cast<InEventPort*>(child);

    if(!outEventPort && !inEventPort){
#ifdef DEBUG_MODE
        qWarning() << "Can only adopt an OutEventPort, an InEventPort or an Attribute in a Component.";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
