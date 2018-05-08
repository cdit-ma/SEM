#ifndef MEDEA_FUNCTION_H
#define MEDEA_FUNCTION_H
#include "../node.h"
#include "containernode.h"

class EntityFactory;
namespace MEDEA{
    class Function :  public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        Function(EntityFactory& factory, bool is_temp_node);

        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_FUNCTION_H
