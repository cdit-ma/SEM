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

        protected:
            void bindWorkerID(Node* child, bool setup);
            void childAdded(Node* child);
            void childRemoved(Node* child); 

            void definitionSet(Node* definition);
    };
};


#endif // WORKERINSTANCE_H