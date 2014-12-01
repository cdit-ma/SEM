#include "assemblydefinitions.h"
#include "componentassembly.h"
#include <qdebug>
AssemblyDefinitions::AssemblyDefinitions():Node()
{

    this->addAspect("Assembly");
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


    if(!component){
        qWarning() << "AssemblyDefinitions can only adopt a ComponentAssembly Node";
        return false;
    }

    return Node::canAdoptChild(child);
}
