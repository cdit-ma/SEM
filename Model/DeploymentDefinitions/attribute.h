#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

#include <Qstring>

class Attribute : public Node
{
    Q_OBJECT
public:
    Attribute(QString name="");
    ~Attribute();

    // GraphML interface
public:
    QString toString();
    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // ATTRIBUTE_H
