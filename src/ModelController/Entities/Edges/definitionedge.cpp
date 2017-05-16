#include "definitionedge.h"
#include "../node.h"
#include "../../edgekinds.h"
#include <QDebug>

DefinitionEdge::DefinitionEdge(Node *src, Node *dst):Edge(src, dst, EDGE_KIND::DEFINITION)
{
}

DefinitionEdge::DefinitionEdge(EntityFactory* factory):Edge(factory, EDGE_KIND::DEFINITION, "Edge_Definition"){
    auto kind = EDGE_KIND::DEFINITION;
	QString kind_string = "Edge_Definition";
	RegisterEdgeKind(factory, kind, kind_string, &DefinitionEdge::ConstructEdge);
}

DefinitionEdge *DefinitionEdge::ConstructEdge(Node *src, Node *dst)
{
    DefinitionEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(EDGE_KIND::DEFINITION, dst)){
            edge = new DefinitionEdge(src, dst);
        }
    }
    return edge;
}
