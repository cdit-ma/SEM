#include "idl.h"
#include "component.h"
#include "blackbox.h"
#include "aggregate.h"
#include "vector.h"

IDL::IDL(): Node()
{
}

IDL::~IDL()
{
}

bool IDL::canAdoptChild(Node *item)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(item);
    BlackBox* blackBox = dynamic_cast<BlackBox*>(item);
    Component* component = dynamic_cast<Component*>(item);
    Vector* vector = dynamic_cast<Vector*>(item);

    if(!(aggregate || blackBox || component || vector)){
        return false;
    }


    foreach(Node* child, getChildren(0)){
        QString childKind = child->getNodeKind();

        if(vector || aggregate){
            if(!(childKind == "Aggregate" || childKind == "Vector")){
                return false;
            }
        }
        else if(component){
            if(childKind != "Component"){
                return false;
            }
        }else if(blackBox){
            if(childKind != "BlackBox"){
                return false;
            }
        }
    }

    return Node::canAdoptChild(item);
}
