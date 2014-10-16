#ifndef BEHAVIOURDEFINITIONS_H
#define BEHAVIOURDEFINITIONS_H

#include "../node.h"

class BehaviourDefinitions: public Node
{
    Q_OBJECT
public:
    BehaviourDefinitions(QString name ="");
    ~BehaviourDefinitions();


    // GraphML interface
public:
    QString toString();

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // BEHAVIOURDEFINITIONS_H
