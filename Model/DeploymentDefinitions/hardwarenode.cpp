#include "hardwarenode.h"
#include "componentassembly.h"
#include "componentinstance.h"
#include <QDebug>

HardwareNode::HardwareNode(QString name):Node()
{
    //qDebug() << "Constructed Hardwdare Node: "<< this->getName();
}

HardwareNode::~HardwareNode()
{

}

bool HardwareNode::canConnect(Node* attachableObject)
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

    return true;
}

bool HardwareNode::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    qWarning() << "Hardware Node cannot adopt any Children.";
    return false;
}

QString HardwareNode::toString()
{
    return QString("HardwareNode[%1]: "+this->getName()).arg(this->getID());
}
