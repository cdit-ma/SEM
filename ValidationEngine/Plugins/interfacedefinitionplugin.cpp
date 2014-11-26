#include "interfacedefinitionplugin.h"
#include <QDebug>


InterfaceDefinitionPlugin::InterfaceDefinitionPlugin()
{

}


bool InterfaceDefinitionPlugin::validate(Model *model)
{
    if(!model){
        return false;
    }

    InterfaceDefinitions* iD = getInterfaceDefinition(model);
    if(!iD){
        return false;
    }

    if(!checkAggregates(iD)){
        return false;
    }


    return true;
}


QString InterfaceDefinitionPlugin::getName()
{
    return "Interface Definition Plugin";
}

InterfaceDefinitions *InterfaceDefinitionPlugin::getInterfaceDefinition(Model *model)
{
    InterfaceDefinitions *returnable = 0;
    foreach(Node* node, model->getChildren(0)){
        InterfaceDefinitions* iD = dynamic_cast<InterfaceDefinitions*>(node);
        if(iD){
            if(returnable){
                printError(0,model ,"Got more than 1 InterfaceDefintion Nodes.");
                return 0;
            }
            returnable = iD;
        }
    }
    if(!returnable){
        printError(1, model, "Got No InterfaceDefintion Nodes.");
    }
    return returnable;
}

bool InterfaceDefinitionPlugin::checkAggregates(InterfaceDefinitions *iD)
{
    QVector<File*> files;

    //Should only have Files inside InterfaceDefinitions

    foreach(Node* node, iD->getChildren(0)){
        if(!node){
            printError(3, iD, "Got a Null Node inside InterfaceDefinition Node.");
            return false;
        }
        //Check for Files
        File* file = dynamic_cast<File*>(node);

        if(file){
            files.append(file);
        }else{
            printError(4, node, "Got a Non File Node inside InterfaceDefintion Nodes.");
            return false;
        }
    }

    //Check for Files.
    if(files.size() == 0){
        printError(5, iD, "No Files defined inside InterfaceDefinition Node!.");
        return false;
    }

    //Check for Unique Names
    QStringList fileNames;
    foreach(File* file, files){
        QString fileName = file->getDataValue("label");
        if(fileNames.contains(fileName)){
            printError(6, file, "All Files must have unique label data. Filename = " + fileName);
            return false;
        }else{
            fileNames.append(fileName);
        }
    }

    return true;

}

