#ifndef EVENTPORTINSTANCE_H
#define EVENTPORTINSTANCE_H
#include "../node.h"
#include "../InterfaceDefinitions/eventport.h"

class EventPortInstance : public Node
{
    Q_OBJECT
public:
    EventPortInstance(bool isInEventPortInstance);
    ~EventPortInstance();

    bool isInEventPortInstance();
    bool isOutEventPortInstance();

    Aggregate* getAggregate();
    EventPort* getEventPortDefinition();

    bool canAdoptChild(Node*);
    bool canConnect_AssemblyEdge(Node *node);
    bool canConnect_DefinitionEdge(Node *definition);
private:
    bool inEventPortInstance;
    bool eventPortDelegate;
};

#endif // EVENTPORTINSTANCE_H
