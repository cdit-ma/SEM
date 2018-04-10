#ifndef WORKERINSTANCE_H
#define WORKERINSTANCE_H

#include "../node.h"

namespace MEDEA{
    class WorkerInstance : public Node{
        public:
            WorkerInstance(EntityFactory* factory);
            WorkerInstance();

            bool canAdoptChild(Node* child);
            bool canAcceptEdge(EDGE_KIND edge_kind, Node* dst);
    };
};


#endif // WORKERINSTANCE_H