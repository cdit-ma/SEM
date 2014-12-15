#include "assemblydefinitions.h"
#include "componentassembly.h"
#include "managementcomponent.h"
#include <qdebug>
AssemblyDefinitions::AssemblyDefinitions():Node()
{
     addAspect("Assembly");
}

AssemblyDefinitions::~AssemblyDefinitions()
{

}

QString AssemblyDefinitions::toString()
{
    return QString("AssemblyDefinitions[%1]: "+this->getName()).arg(this->getID());
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
        qWarning() << "AssemblyDefinitions can only adopt a ComponentAssembly or ManagementComponent Node";
        return false;
    }

    if(managementComponent){
        QString type = managementComponent->getDataValue("type");

        foreach(Node* cChild, getChildren(0)){
            ManagementComponent* mChild = dynamic_cast<ManagementComponent *>(cChild);

            if(mChild->getDataValue("type") == type){
                return false;
            }
        }
    }

    return Node::canAdoptChild(child);
}
