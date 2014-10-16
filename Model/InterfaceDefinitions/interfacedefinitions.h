#ifndef INTERFACEDEFINITIONS_H
#define INTERFACEDEFINITIONS_H

#include "../node.h"

class InterfaceDefinitions: public Node
{
    Q_OBJECT
public:
    InterfaceDefinitions(QString name ="");
    ~InterfaceDefinitions();

    // GraphML interface
public:
    QString toString();

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // INTERFACEDEFINITIONS_H
