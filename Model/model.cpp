#include "model.h"
#include <QDebug>


Model::Model(): Node()
{
    qCritical() << "Constructed Model.";
    addAspect("Definitions");
    addAspect("Workload");
    addAspect("Deployment");
}

Model::~Model()
{
    removeEdges();
    removeChildren();
    qCritical() << "Destructed Model.";
}

QString Model::toString()
{
    return QString("Model[%1]: "+this->getName()).arg(this->getID());
}

bool Model::canAdoptChild(Node *child)
{
    InterfaceDefinitions* interfaceDefinitions = dynamic_cast<InterfaceDefinitions*>(child);
    DeploymentDefinitions* deploymentDefinitions = dynamic_cast<DeploymentDefinitions*>(child);
    BehaviourDefinitions* behaviourDefinitions = dynamic_cast<BehaviourDefinitions*>(child);

    if(!interfaceDefinitions && !deploymentDefinitions && !behaviourDefinitions){
        qWarning() << "Model can only adopt interfaceDefinitions, deploymentDefinitions, interfaceDefinitions";
        return false;
    }

    return Node::canAdoptChild(child);
}

QString Model::toGraphML(qint32 indentationLevel)
{
    return Node::toGraphML(indentationLevel);
}

bool Model::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}
