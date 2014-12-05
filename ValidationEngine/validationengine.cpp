#include "validationengine.h"
#include <QDebug>
ValidationEngine::ValidationEngine()
{

}


void ValidationEngine::addPlugin(ValidationPlugin *plugin)
{
    if(plugin && !plugins.contains(plugin)){
        //qDebug()<< "Added Plugin: " << plugin->getName();
        plugins.append(plugin);
    }
}

bool ValidationEngine::validateModel(Model *model)
{
    foreach(ValidationPlugin* vP, plugins){
        qDebug() << "Validating Model against " << vP->getName();
        bool result = vP->validate(model);
        if(!result){
            qCritical() << "Validation failed!";
            return false;
        }
    }
    return true;
}
