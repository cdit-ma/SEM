#ifndef BEHAVIOURNODEADAPTER_H
#define BEHAVIOURNODEADAPTER_H
#include "nodeadapter.h"
#include "../Model/node.h"
#include "../Model/BehaviourDefinitions/behaviournode.h"

class BehaviourNodeAdapter : public NodeAdapter
{
public:
    BehaviourNodeAdapter(BehaviourNode* node);
    bool needsConnection();
private:
    BehaviourNode* getBehaviourNode();
};
#endif // NODEADAPTER_H
