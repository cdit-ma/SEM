#include "variable.h"
#include <QDebug>
#include "../InterfaceDefinitions/aggregateinstance.h"
#include "../InterfaceDefinitions/member.h"
#include "../InterfaceDefinitions/aggregate.h"
#include "../InterfaceDefinitions/vectorinstance.h"

Variable::Variable():BehaviourNode()
{

}

Variable::~Variable()
{
}

bool Variable::canConnect(Node*)
{
    return false;
}

bool Variable::canAdoptChild(Node* child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);

    if(!aggregateInstance && !vectorInstance){
#ifdef DEBUG_MODE
        qWarning() << "Aggregate can only adopt Member/Instances";
#endif
        return false;
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
