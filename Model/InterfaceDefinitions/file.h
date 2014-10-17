#ifndef FILE_H
#define FILE_H
#include "../node.h"

class File : public Node
{
    Q_OBJECT
public:
    File(QString name ="");
    ~File();

    // GraphML interface
public:
    QString toString();

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // FILE_H
