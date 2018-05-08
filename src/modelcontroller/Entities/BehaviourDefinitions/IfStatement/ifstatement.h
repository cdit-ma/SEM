#ifndef MEDEA_IFSTATEMENT_H
#define MEDEA_IFSTATEMENT_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class IfStatement: public Node
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        IfStatement(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};
#endif // MEDEA_IFSTATEMENT_H
