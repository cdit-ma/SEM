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
    Q_UNUSED(attachableObject);
    return false;
}

bool HardwareCluster::canAdoptChild(Node *child)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (child);

     //Is this child a HardwareNode?
    if(!hardwareNode){
        qWarning() << "HardwareCluster can only adopt HardwareNodes";
        return false;
    }

    return Node::canAdoptChild(child);
}
