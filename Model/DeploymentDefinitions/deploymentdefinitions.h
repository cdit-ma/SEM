#ifndef DEPLOYMENTDEFINITIONS_H
#define DEPLOYMENTDEFINITIONS_H

#include "../node.h"

class DeploymentDefinitions: public Node
{
    Q_OBJECT
public:
    DeploymentDefinitions(QString name ="");
    ~DeploymentDefinitions();


    // GraphML interface
public:
    QString toString();

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};
#endif // DEPLOYMENTDEFINITIONS_H
