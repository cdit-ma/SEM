#include "edgefactory.h"

#include <QDebug>
#include "Edges/aggregateedge.h"
#include "Edges/assemblyedge.h"
#include "Edges/dataedge.h"
#include "Edges/definitionedge.h"
#include "Edges/deploymentedge.h"
#include "Edges/qosedge.h"
#include "Edges/workflowedge.h"

EdgeFactory* EdgeFactory::factory = 0;

Edge *EdgeFactory::createEdge(Node *source, Node *destination, Edge::EDGE_KIND edgeKind)
{
    return getFactory()->_createEdge(source, destination, edgeKind);
}

Edge *EdgeFactory::createEdge(Node *source, Node *destination, QString kind)
{
    return createEdge(source, destination, getEdgeKind(kind));
}

EdgeFactory *EdgeFactory::getFactory()
{
    if(factory == 0){
        factory = new EdgeFactory();
    }
    return factory;
}

Edge *EdgeFactory::_createEdge(Node *source, Node *destination, Edge::EDGE_KIND edgeKind)
{
    Edge* edge = 0;

    if(source && destination){
        switch(edgeKind){
           case Edge::EC_DEFINITION:
               edge = DefinitionEdge::createDefinitionEdge(source, destination);
               break;
           case Edge::EC_WORKFLOW:
               edge = WorkflowEdge::createWorkflowEdge(source, destination);
               break;
           case Edge::EC_DATA:
               edge = DataEdge::createDataEdge(source, destination);
               break;
           case Edge::EC_ASSEMBLY:
               edge = AssemblyEdge::createAssemblyEdge(source, destination);
               break;
           case Edge::EC_AGGREGATE:
               edge = AggregateEdge::createAggregateEdge(source, destination);
               break;
           case Edge::EC_DEPLOYMENT:
               edge = DeploymentEdge::createDeploymentEdge(source, destination);
               break;
           case Edge::EC_QOS:
               edge = QOSEdge::createQOSEdge(source, destination);
               break;
           default:
               qCritical() << "Edge Kind: " << edgeKind << " Not Implemented!";
               break;
        }
    }
    return edge;
}

QString EdgeFactory::getEdgeKindString(Edge::EDGE_KIND edgeKind)
{
    return getFactory()->edgeLookup.value(edgeKind);
}


QList<Edge::EDGE_KIND> EdgeFactory::getEdgeKinds()
{
    return getFactory()->edgeLookup.keys();
}

Edge::EDGE_KIND EdgeFactory::getEdgeKind(QString kind)
{
    return getFactory()->edgeLookup.key(kind, Edge::EC_UNDEFINED);
}

EdgeFactory::EdgeFactory()
{
    //Add Concrete types
    edgeLookup.insert(Edge::EC_DEFINITION, "Edge_Definition");
    edgeLookup.insert(Edge::EC_AGGREGATE, "Edge_Aggregate");
    edgeLookup.insert(Edge::EC_WORKFLOW, "Edge_Workflow");
    edgeLookup.insert(Edge::EC_ASSEMBLY, "Edge_Assembly");
    edgeLookup.insert(Edge::EC_DATA, "Edge_Data");
    edgeLookup.insert(Edge::EC_DEPLOYMENT, "Edge_Deployment");
    edgeLookup.insert(Edge::EC_QOS, "Edge_QOS");
}
