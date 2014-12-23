#include "hardwarenode.h"
#include "componentassembly.h"
#include "componentinstance.h"
#include "managementcomponent.h"
#include <QDebug>

HardwareNode::HardwareNode(QString name):Node()
{
    //qDebug() << "Constructed Hardwdare Node: "<< this->getName();
}

HardwareNode::~HardwareNode()
{

}

//Hardware Node cannot connect to anything.
bool HardwareNode::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

//Hardware Node cannot adopt anything.
bool HardwareNode::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}

QString HardwareNode::toString()
{
    return QString("HardwareNode[%1]: "+this->getName()).arg(this->getID());
}
