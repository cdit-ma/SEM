#include "componentassembly.h"
#include "componentinstance.h"
#include "hardwarecluster.h"
#include "hardwarenode.h"
#include "ineventportdelegate.h"
#include "../edge.h"
#include "outeventportdelegate.h"
#include <QDebug>
ComponentAssembly::ComponentAssembly():Node()
{
}

ComponentAssembly::~ComponentAssembly()
{
    //Destructor
}

bool ComponentAssembly::canAdoptChild(Node *child)
{
    ComponentAssembly* componentAssembly = dynamic_cast<ComponentAssembly*>(child);
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*> (child);
    InEventPortDelegate* inEventPortDelegate = dynamic_cast<InEventPortDelegate*> (child);
    OutEventPortDelegate* outEventPortDelegate = dynamic_cast<OutEventPortDelegate*> (child);

    //Is this child a ComponentInstance?
    if(!componentInstance && !inEventPortDelegate && !outEventPortDelegate && !componentAssembly){
        qWarning() << "Assembly Node can only adopt a ComponentInstance, ComponentAssembly, InEventPortDelegate or OutEventPortDelegate Node";
        return false;
    }

    return Node::canAdoptChild(child);
}

//A ComponentAssembly can be connected to:
//Deployed to:
// + HardwareCluster
// + HardwareNode
bool ComponentAssembly::canConnect(Node* attachableObject)
{
    HardwareCluster* hardwareCluster = dynamic_cast<HardwareCluster*> (attachableObject);
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

    if(!hardwareNode && !hardwareCluster){
        qWarning() << "ComponentInstance Node can only be connected to a HardwareNode, HardwareCluster or a Component";
        return false;
    }

    if(hardwareCluster || hardwareNode){
        //Check for deployment edges already.
        foreach(Edge* edge, getEdges(0)){
            if(edge->isDeploymentLink()){
                qWarning() << "ComponentInstance Node is already deployed!";
                return false;
            }
        }
    }

    return Node::canConnect(attachableObject);
}
