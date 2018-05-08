#ifndef MEDEA_CLASS_H
#define MEDEA_CLASS_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class Class : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        Class(EntityFactory& factory, bool is_temp_node);
    };
};

#endif // MEDEA_CLASS_H
