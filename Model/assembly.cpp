#include "assembly.h"
#include "componentinstance.h"
#include <QDebug>
Assembly::Assembly(QString name):Node(this->nodeKind,name)
{
    qDebug() << "Constructed Assembly: "<< this->getName();
}

Assembly::~Assembly()
{
    //Destructor
}

bool Assembly::isAdoptLegal(GraphML *child)
{
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*> (child);

    //Is this child a ComponentInstance?
    if(componentInstance == 0){
        qWarning() << "Assembly Node can only adopt a Component Instance Node";
        return false;
    }

    if(this->getGraph() != NULL){
        return this->getGraph()->isAdoptLegal(child);
    }
    return true;
}

bool Assembly::isEdgeLegal(GraphML *attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

     //Is this child a HardwareNode?
    if(hardwareNode == 0){
        //Check stuff!
        qWarning() << "AssemblyNode can only connect to a HardwareNode";
        return false;
    }

    //Check for existing connection.
    if(this->isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    return true;
}

QString Assembly::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());
    returnable += tabSpace + "\t" + QString("<data key =\"name\">%1</data>\n").arg(this->getName());
    returnable += tabSpace + "\t" + QString("<data key =\"type\">Assembly</data>\n");
    returnable += Node::toGraphML(indentationLevel+1);
    returnable += tabSpace + "</node>\n";
    return returnable;
}

QString Assembly::toString()
{
    return QString("Assembly[%1]: "+this->getName()).arg(this->getID());
}
