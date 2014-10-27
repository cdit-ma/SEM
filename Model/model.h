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


#include "BehaviourDefinitions/periodicevent.h"

#include "DeploymentDefinitions/deploymentdefinitions.h"
#include "DeploymentDefinitions/componentassembly.h"

#include "DeploymentDefinitions/attributeinstance.h"
#include "DeploymentDefinitions/componentinstance.h"
#include "DeploymentDefinitions/ineventportinstance.h"
#include "DeploymentDefinitions/outeventportinstance.h"

#include "DeploymentDefinitions/hardwarecluster.h"
#include "DeploymentDefinitions/hardwarenode.h"
#include "DeploymentDefinitions/member.h"

#include "InterfaceDefinitions/interfacedefinitions.h"
#include "InterfaceDefinitions/attribute.h"
#include "InterfaceDefinitions/component.h"
#include "InterfaceDefinitions/outeventport.h"
#include "InterfaceDefinitions/ineventport.h"
#include "InterfaceDefinitions/file.h"
#include "InterfaceDefinitions/aggregate.h"
#include "InterfaceDefinitions/member.h"
#include "InterfaceDefinitions/aggregatemember.h"




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
    QString toString();

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
