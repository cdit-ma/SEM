#include "model.h"
#include <QDebug>


Model::Model(): Node()
{
    setTop(0);
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

    if(!interfaceDefinitions && !deploymentDefinitions && !behaviourDefinitions){
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

Edge::EDGE_CLASS Model::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}
