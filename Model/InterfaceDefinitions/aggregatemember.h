#ifndef AGGREGATEMEMBER_H
#define AGGREGATEMEMBER_H
#include "../node.h"

class Aggregate;

class AggregateMember : public Node
{
    Q_OBJECT
public:
    AggregateMember(QString name ="");
    ~AggregateMember();

    void setDefinition(Aggregate* def);
    Aggregate* getDefinition();
    // GraphML interface
public:
    QString toString();

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);

private:
    Aggregate* def;
};



#endif // AGGREGATEMEMBER_H
