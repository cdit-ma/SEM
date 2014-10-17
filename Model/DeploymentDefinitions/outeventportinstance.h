#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventport.h"
#include "../InterfaceDefinitions/outeventport.h"

class OutEventPortInstance : public EventPort
{
        Q_OBJECT
public:
    OutEventPortInstance(QString name="");
    ~OutEventPortInstance();
    // GraphML interface
public:
    void setDefinition(OutEventPort* def);
    OutEventPort* getDefinition();

    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();

private:
    OutEventPort* def;
};

#endif // OUTEVENTPORTINSTANCE_H
