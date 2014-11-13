#ifndef INTERFACEDEFINITIONPLUGIN_H
#define INTERFACEDEFINITIONPLUGIN_H
#include "../validationplugin.h"
#include <QString>

//Plugin Interface
class InterfaceDefinitionPlugin: public ValidationPlugin{

public:
    InterfaceDefinitionPlugin();
    bool validate(Model *model);
    QString getName();

private:
    //Checks 0
    InterfaceDefinitions* getInterfaceDefinition(Model* model);
    bool checkAggregates(InterfaceDefinitions* iD);
    InterfaceDefinitions* interfaceDefintion;
};

#endif // VALIDATIONPLUGIN_H


