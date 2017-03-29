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
        auto eS = edgeLookup.value(edgeKind, 0);

        if(eS){
            edge = eS->constructor(source, destination);
        }else{
            qCritical() << "Edge Kind: " << getEdgeKindString(edgeKind) << " Not Implemented!";
        }
    }
    return edge;
}

void EdgeFactory::addKind(Edge::EDGE_KIND edge_kind, QString edge_kind_str, std::function<Edge *(Node *, Node *)> constructor)
{
    if(!edgeLookup.contains(edge_kind)){
        auto eS = new EdgeLookupStruct();
        eS->kind = edge_kind;
        eS->kind_str = edge_kind_str;
        eS->constructor = constructor;
        edgeLookup.insert(edge_kind, eS);

        if(!edgeKindLookup.contains(edge_kind_str)){
            edgeKindLookup.insert(edge_kind_str, edge_kind);
        }
    }
}

QString EdgeFactory::getEdgeKindString(Edge::EDGE_KIND edgeKind)
{
    QString kind;
    auto eS = getFactory()->edgeLookup.value(edgeKind, 0);
    if(eS){
        kind = eS->kind_str;
    }
    return kind;
}


QList<Edge::EDGE_KIND> EdgeFactory::getEdgeKinds()
{
    return getFactory()->edgeLookup.keys();
}

Edge::EDGE_KIND EdgeFactory::getEdgeKind(QString kind)
{
    return getFactory()->edgeKindLookup.value(kind, Edge::EC_NONE);
}

EdgeFactory::EdgeFactory()
{
    //Add all types
    addKind(Edge::EC_DEFINITION, "Edge_Definition", &DefinitionEdge::createDefinitionEdge);
    addKind(Edge::EC_AGGREGATE, "Edge_Aggregate", &AggregateEdge::createAggregateEdge);
    addKind(Edge::EC_WORKFLOW, "Edge_Workflow", &WorkflowEdge::createWorkflowEdge);
    addKind(Edge::EC_ASSEMBLY, "Edge_Assembly", &AssemblyEdge::createAssemblyEdge);
    addKind(Edge::EC_DATA, "Edge_Data", &DataEdge::createDataEdge);
    addKind(Edge::EC_DEPLOYMENT, "Edge_Deployment", &DeploymentEdge::createDeploymentEdge);
    addKind(Edge::EC_QOS, "Edge_QOS", &QOSEdge::createQOSEdge);
}

EdgeFactory::~EdgeFactory()
{

}
