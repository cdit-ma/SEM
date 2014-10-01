#include "InEventPortIDL.h"
#include <QDebug>
#include <typeinfo>
#include "outeventportidl.h"
#include "member.h"

InEventPortIDL::InEventPortIDL(QString name):EventPort(name)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

InEventPortIDL::~InEventPortIDL()
{
   // foreach(InEventPort* child, inEventPorts){
    //    delete child;
   // }

}

void InEventPortIDL::addInEventPortInstance(InEventPort *port)
{
    if(port == 0){
       qCritical() << "Cannot adopt a NULL InEventPort.";
    }else{
        if(!inEventPorts.contains(port)){
            inEventPorts.append(port);
            port->setInEventPortIDL(this);
        }
    }

}

void InEventPortIDL::removeInEventPortInstance(InEventPort *port)
{
    qCritical() << "Removing Port";
    if(inEventPorts.contains(port)){
        inEventPorts.removeAt(inEventPorts.indexOf(port));
    }
}

QVector<InEventPort *> InEventPortIDL::getInEventPortInstances()
{
    return inEventPorts;
}

bool InEventPortIDL::isAdoptLegal(GraphMLContainer *child)
{
    Member* member = dynamic_cast<Member*> (child);

    if(member == 0){
        qCritical() << "Cannot connect to anything which isn't a Member.";
        return false;
    }

    if(this->isAncestorOf(child) || this->isDescendantOf(child)){
        qCritical() << "Already related to this node.";
        return false;
    }

    return true;
}

bool InEventPortIDL::isEdgeLegal(GraphMLContainer *attachableObject)
{
    OutEventPortIDL* outputEventPort = dynamic_cast<OutEventPortIDL*> (attachableObject);

    InEventPort* inEventPort = dynamic_cast<InEventPort*>(attachableObject);

    if(outputEventPort == 0 && inEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Output Event Port or InEventPort instance";
        return false;
    }

    if(outputEventPort){
        if(outputEventPort->getDataValue("type") != this->getDataValue("type")){
            qCritical() << "Cannot connect 2 Different IDL Types!";
            return false;
        }

        if(outputEventPort->getParent() == this->getParent()){
            //qCritical() << "Cannot connect 2 Ports from the same component!";
            //return false;
        }
    }

    if(inEventPort){

    }

    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    return true;
}


QString InEventPortIDL::toString()
{
    return QString("InEventPortIDL[%1]: "+this->getName()).arg(this->getID());

}

void InEventPortIDL::addEdge(Edge *edge)
{
    //Make sure if we are restoring a
    InEventPort* src = dynamic_cast<InEventPort*>(edge->getSource());
    InEventPort* dst = dynamic_cast<InEventPort*>(edge->getDestination());

    if(src != 0){
        this->addInEventPortInstance(src);
    }
    if(dst != 0){
        this->addInEventPortInstance(dst);
    }

    Node::addEdge(edge);

}
