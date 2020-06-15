#include "assemblyedge.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const auto edge_kind = EDGE_KIND::ASSEMBLY;
const QString kind_string = "Edge_Assembly";

void AssemblyEdge::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(edge_kind, kind_string, [](EntityFactoryBroker& broker, Node* src, Node* dst){
        return new AssemblyEdge(broker, src, dst);
        });
}

AssemblyEdge::AssemblyEdge(EntityFactoryBroker& broker, Node* src, Node* dst) : Edge(broker, src, dst, edge_kind){
};
