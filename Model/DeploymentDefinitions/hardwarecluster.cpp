#include "hardwarecluster.h"
#include "hardwarenode.h"
#include "componentassembly.h"
#include "componentinstance.h"
#include <QDebug>

HardwareCluster::HardwareCluster(QString name):Node()
{
    //qDebug() << "Constructed Hardwdare Cluster: "<< this->getName();
}

HardwareCluster::~HardwareCluster()
{

}

QString HardwareCluster::toString()
{
    return QString("HardwareCluster[%1]: "+this->getName()).arg(this->getID());
}

bool HardwareCluster::canConnect(Node* attachableObject)
{
    ComponentAssembly* assembly = dynamic_cast<ComponentAssembly*> (attachableObject);
    ComponentInstance* instance = dynamic_cast<ComponentInstance*> (attachableObject);

     //Is this child a HardwareNode?
    if(assembly == 0 && instance == 0){
        //Check stuff!
        qWarning() << "AssemblyNode can only connect to either a Component Assembly or a Component Instance";
        return false;
    }

    //Check for existing connection.
    if(isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool HardwareCluster::canAdoptChild(Node *child)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (child);

     //Is this child a HardwareNode?
    if(hardwareNode == 0 ){
        //Check stuff!
        qWarning() << "HardwareCluster can only adopt a HardwareNode";
        return false;
    }

    return Node::canAdoptChild(child);

}
