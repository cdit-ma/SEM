#ifndef VALIDATIONENGINE_H
#define VALIDATIONENGINE_H
#include <QString>
#include "validationplugin.h"
#include "../Model/model.h"

class ValidationEngine
{
public:
    ValidationEngine();
    ~ValidationEngine();
    void addPlugin(ValidationPlugin* plugin);
    bool validateModel(Model* model);
private:
    QList<ValidationPlugin*> plugins;
};




#endif // VALIDATIONENGINE_H
