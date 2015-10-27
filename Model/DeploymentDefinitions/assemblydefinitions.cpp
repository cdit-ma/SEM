#include "assemblydefinitions.h"
#include "componentassembly.h"
#include "managementcomponent.h"
#include <QDebug>
AssemblyDefinitions::AssemblyDefinitions():Node(Node::NT_ASPECT)
{
}

AssemblyDefinitions::~AssemblyDefinitions()
{

}

bool AssemblyDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool AssemblyDefinitions::canAdoptChild(Node *child)
{

    ComponentAssembly* component = dynamic_cast<ComponentAssembly *>(child);
    ManagementComponent* managementComponent = dynamic_cast<ManagementComponent *>(child);

    if(!component && !managementComponent){
#ifdef DEBUG_MODE
        qWarning() << "AssemblyDefinitions can only adopt a ComponentAssembly and ManagementComponent Node";
#endif
        return false;
    }

    if(managementComponent){
        QString type = managementComponent->getDataValue("type");

        foreach(Node* cChild, getChildren(0)){
            ManagementComponent* mChild = dynamic_cast<ManagementComponent *>(cChild);

            if(mChild && mChild->getDataValue("type") == type){
#ifdef DEBUG_MODE
                qWarning() << "AssemblyDefinitions can only adopt 1 of a particularly typed ManagementComponent!";
#endif
                return false;
            }
        }
    }

    return Node::canAdoptChild(child);
}
