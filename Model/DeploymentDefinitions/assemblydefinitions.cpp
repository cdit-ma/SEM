#include "assemblydefinitions.h"
#include "componentassembly.h"
#include "managementcomponent.h"
#include "blackboxinstance.h"
#include <QDebug>
AssemblyDefinitions::AssemblyDefinitions():Node()
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
    BlackBoxInstance* blackBoxInstance = dynamic_cast<BlackBoxInstance *>(child);


    if(!component && !managementComponent && !blackBoxInstance){
#ifdef DEBUG_MODE
        qWarning() << "AssemblyDefinitions can only adopt a ComponentAssembly, ManagementComponent or BlackBoxInstance Node";
#endif
        return false;
    }

    if(managementComponent){
        QString type = managementComponent->getDataValue("type");

        foreach(Node* cChild, getChildren(0)){
            ManagementComponent* mChild = dynamic_cast<ManagementComponent *>(cChild);

            if(mChild->getDataValue("type") == type){
#ifdef DEBUG_MODE
         qWarning() << "AssemblyDefinitions can only adopt 1 of a particularly typed ManagementComponent!";
#endif
                return false;
            }
        }
    }

    return Node::canAdoptChild(child);
}
