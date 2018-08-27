#ifndef MEDEA_FUNCTION_H
#define MEDEA_FUNCTION_H
#include "../node.h"
#include "containernode.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class Function :  public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        Function(EntityFactoryBroker& factory, bool is_temp_node);
        QSet<Node*> getDependants() const;

        void parentSet(Node* parent);
    public:

        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_FUNCTION_H
