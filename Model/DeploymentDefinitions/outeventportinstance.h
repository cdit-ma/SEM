#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "../InterfaceDefinitions/outeventport.h"

class OutEventPortInstance : public Node
{
        Q_OBJECT
public:
    OutEventPortInstance(QString name="");
    ~OutEventPortInstance();
    // GraphML interface
public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    
};

#endif // OUTEVENTPORTINSTANCE_H
