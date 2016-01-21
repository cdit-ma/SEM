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
    bool needsDataInput();
    bool needsDataOutput();

    bool needsConnection();
private:
    BehaviourNode* getBehaviourNode();
};
#endif // NODEADAPTER_H
