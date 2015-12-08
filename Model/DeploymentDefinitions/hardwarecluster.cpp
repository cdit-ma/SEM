#include "hardwarecluster.h"
#include "hardwarenode.h"
#include "componentassembly.h"
#include "componentinstance.h"
#include <QDebug>

HardwareCluster::HardwareCluster():Node()
{
    //qDebug() << "Constructed Hardwdare Cluster: "<< this->getName();
}

HardwareCluster::~HardwareCluster()
{

}

Edge::EDGE_CLASS HardwareCluster::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool HardwareCluster::canAdoptChild(Node *child)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (child);

     //Is this child a HardwareNode?
    if(!hardwareNode){
//#ifdef DEBUG_MODE
        //qWarning() << "HardwareCluster can only adopt HardwareNodes";
//#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
