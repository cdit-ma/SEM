#include "componentassembly.h"
#include "blackboxinstance.h"
#include "componentinstance.h"
#include "eventportdelegate.h"
#include "hardware.h"

ComponentAssembly::ComponentAssembly():Node()
{
    addValidEdgeType(Edge::EC_DEPLOYMENT);
}

ComponentAssembly::~ComponentAssembly()
{
}

bool ComponentAssembly::canAdoptChild(Node *child)
{
    BlackBoxInstance* blackBoxInstance = dynamic_cast<BlackBoxInstance*> (child);
    ComponentAssembly* componentAssembly = dynamic_cast<ComponentAssembly*>(child);
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*>(child);
    EventPortDelegate* eventPortDelegate = dynamic_cast<EventPortDelegate*>(child);

    if(!(blackBoxInstance || componentAssembly || componentInstance || eventPortDelegate)){
        return false;
    }

    return Node::canAdoptChild(child);
}
