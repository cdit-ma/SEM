#ifndef MODEL_H
#define MODEL_H


#include "edge.h"
#include "node.h"
#include "blanknode.h"
#include "graphmlkey.h"
#include "graphmldata.h"


#include "BehaviourDefinitions/behaviourdefinitions.h"
#include "BehaviourDefinitions/componentimpl.h"
#include "BehaviourDefinitions/ineventportimpl.h"
#include "BehaviourDefinitions/outeventportimpl.h"
#include "BehaviourDefinitions/attributeimpl.h"

#include "BehaviourDefinitions/branchstate.h"
#include "BehaviourDefinitions/condition.h"
#include "BehaviourDefinitions/periodicevent.h"
#include "BehaviourDefinitions/process.h"
#include "BehaviourDefinitions/termination.h"
#include "BehaviourDefinitions/variable.h"
#include "BehaviourDefinitions/workload.h"
#include "BehaviourDefinitions/whileloop.h"



#include "BehaviourDefinitions/periodicevent.h"

#include "DeploymentDefinitions/deploymentdefinitions.h"
#include "DeploymentDefinitions/hardwaredefinitions.h"
#include "DeploymentDefinitions/assemblydefinitions.h"
#include "DeploymentDefinitions/deploymentdefinitions.h"


#include "DeploymentDefinitions/componentassembly.h"

#include "DeploymentDefinitions/attributeinstance.h"
#include "DeploymentDefinitions/componentinstance.h"
#include "DeploymentDefinitions/ineventportinstance.h"
#include "DeploymentDefinitions/outeventportinstance.h"

#include "DeploymentDefinitions/hardwarecluster.h"
#include "DeploymentDefinitions/hardwarenode.h"
#include "DeploymentDefinitions/managementcomponent.h"
#include "DeploymentDefinitions/ineventportdelegate.h"
#include "DeploymentDefinitions/outeventportdelegate.h"

#include "InterfaceDefinitions/interfacedefinitions.h"
#include "InterfaceDefinitions/aggregateinstance.h"

#include "InterfaceDefinitions/memberinstance.h"
#include "InterfaceDefinitions/attribute.h"
#include "InterfaceDefinitions/component.h"
#include "InterfaceDefinitions/outeventport.h"
#include "InterfaceDefinitions/ineventport.h"
#include "InterfaceDefinitions/idl.h"
#include "InterfaceDefinitions/aggregate.h"
#include "InterfaceDefinitions/member.h"

#include "InterfaceDefinitions/blackbox.h"
#include "DeploymentDefinitions/blackboxinstance.h"



#include <QStringList>

#include <QString>
#include <QVector>
#include <QThread>
#include <QObject>
#include <QXmlStreamReader>

class Model: public Node
{
    Q_OBJECT
public:
    Model();
    ~Model();

    // GraphML interface
public:


    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:

    // GraphML interface
public:
    QString toGraphML(qint32 indentationLevel);
};
#endif // MODEL_H
