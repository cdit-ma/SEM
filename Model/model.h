#ifndef MODEL_H
#define MODEL_H

#include "graph.h"
#include "edge.h"
#include "node.h"
#include "blanknode.h"
#include "graphmlkey.h"
#include "graphmldata.h"


#include "BehaviourDefinitions/component.h"
#include "BehaviourDefinitions/ineventport.h"
#include "BehaviourDefinitions/outeventport.h"
#include "BehaviourDefinitions/periodicevent.h"
#include "DeploymentDefinitions/attribute.h"
#include "DeploymentDefinitions/componentassembly.h"
#include "DeploymentDefinitions/componentinstance.h"
#include "DeploymentDefinitions/eventport.h"
#include "DeploymentDefinitions/hardwarecluster.h"
#include "DeploymentDefinitions/hardwarenode.h"
#include "DeploymentDefinitions/ineventportidl.h"
#include "DeploymentDefinitions/member.h"
#include "DeploymentDefinitions/outeventportidl.h"
#include "DeploymentDefinitions/deploymentdefinitions.h"
#include "BehaviourDefinitions/behaviourdefinitions.h"
#include "InterfaceDefinitions/interfacedefinitions.h"




#include <QStringList>

#include <QString>
#include <QVector>
#include <QThread>
#include <QObject>
#include <QXmlStreamReader>

class Model: public GraphMLContainer
{
    Q_OBJECT
public:
    Model(QString name);
    ~Model();


    // GraphML interface
public:
    QString toString();

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);

private:

    // GraphML interface
public:
    QString toGraphML(qint32 indentationLevel);
};
#endif // MODEL_H
