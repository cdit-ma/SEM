#ifndef BEHAVIOURNODEADAPTER_H
#define BEHAVIOURNODEADAPTER_H
#include <QObject>
#include "nodeadapter.h"
#include "../Model/node.h"
#include "../Model/BehaviourDefinitions/behaviournode.h"

class BehaviourNodeAdapter : public NodeAdapter
{
    Q_OBJECT
public:
    BehaviourNodeAdapter(BehaviourNode* node);

    bool hasDataOutput();
    bool hasDataInput();

    bool needsDataInput();
    bool needsDataOutput();


    bool needsLeftEdge();

    bool needsConnection();
private:
    BehaviourNode* getBehaviourNode();
};
#endif // NODEADAPTER_H
