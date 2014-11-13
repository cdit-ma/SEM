#ifndef VALIDATIONENGINE_H
#define VALIDATIONENGINE_H
#include <QString>
#include "validationplugin.h"
#include "../Model/model.h"

class ValidationEngine
{
    //Q_OBJECT
public:
    ValidationEngine();
    void addPlugin(ValidationPlugin* plugin);
    bool validateModel(Model* model);
private:
    QVector<ValidationPlugin*> plugins;
};




#endif // VALIDATIONENGINE_H
