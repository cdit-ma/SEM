#ifndef VALIDATIONPLUGIN_H
#define VALIDATIONPLUGIN_H
#include <QString>
#include "../Model/model.h"

//Plugin Interface
class ValidationPlugin: public QObject{
    Q_OBJECT
public:
    virtual bool validate(Model* model) = 0;
    virtual QString getName() = 0;
    virtual void printError(int checkID, Node* node, QString errorDescription);

signals:
    void highlightNodeError(Node* node, QString error);
};

#endif // VALIDATIONPLUGIN_H
