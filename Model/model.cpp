#include "model.h"
#include <QDebug>


Model::Model(): Node()
{
    setTop();
}

Model::~Model()
{
    removeEdges();
    removeChildren();
}

bool Model::canAdoptChild(Node *child)
{
    InterfaceDefinitions* interfaceDefinitions = dynamic_cast<InterfaceDefinitions*>(child);
    DeploymentDefinitions* deploymentDefinitions = dynamic_cast<DeploymentDefinitions*>(child);
    BehaviourDefinitions* behaviourDefinitions = dynamic_cast<BehaviourDefinitions*>(child);
    WorkerDefinitions* workerDefinitions = dynamic_cast<WorkerDefinitions*>(child);

    if(!interfaceDefinitions && !deploymentDefinitions && !behaviourDefinitions && !workerDefinitions){
        #ifdef DEBUG_MODE
        qWarning() << "Model can only adopt interfaceDefinitions, deploymentDefinitions, interfaceDefinitions";
#endif
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
