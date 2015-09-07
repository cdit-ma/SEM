#include "blackboxinstance.h"
#include <QDebug>
#include "../InterfaceDefinitions/blackbox.h"
#include "outeventportinstance.h"
#include "ineventportinstance.h"
#include "hardwarecluster.h"
#include "hardwarenode.h"

BlackBoxInstance::BlackBoxInstance():Node(Node::NT_INSTANCE)
{
}

BlackBoxInstance::~BlackBoxInstance()
{

}

bool BlackBoxInstance::canAdoptChild(Node *child)
{
    OutEventPortInstance* outEventPortInstance = dynamic_cast<OutEventPortInstance*>(child);
    InEventPortInstance* inEventPortInstance = dynamic_cast<InEventPortInstance*>(child);


    if(!(outEventPortInstance || inEventPortInstance)){
#ifdef DEBUG_MODE
        qWarning() << "BlackBox Instance can only Adopt a OutEventPortInstance or InEventPortInstance.";
#endif
        return false;
    }

    return Node::canAdoptChild(child);

}

//A ComponentInstance can be connected to:
//Connected to a Definition:
// + Component (If it has no definition already)
bool BlackBoxInstance::canConnect(Node *attachableObject)
{
     BlackBox* blackBox = dynamic_cast<BlackBox*> (attachableObject);
     HardwareCluster* hardwareCluster = dynamic_cast<HardwareCluster*> (attachableObject);
     HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

     if(!blackBox && !hardwareCluster && !hardwareNode){
#ifdef DEBUG_MODE
        qWarning() << "BlackBoxInstance Node can only be connected to a BlackBox Definition, or Deployed to a HardwareCluster or HardwareNode.";
#endif
         return false;
     }
     if(blackBox && getDefinition()){
#ifdef DEBUG_MODE
        qWarning() << "BlackBoxInstance Node can only be connected to one BlackBox.";
#endif
         return false;
     }

     if(hardwareCluster || hardwareNode){
         //Check for deployment edges already.
         foreach(Edge* edge, getEdges(0)){
             if(edge->isDeploymentLink()){
 #ifdef DEBUG_MODE
                 qWarning() << "BlackBoxInstance Node is already deployed!";
 #endif
                 return false;
             }
         }
     }

     return Node::canConnect(attachableObject);
}
