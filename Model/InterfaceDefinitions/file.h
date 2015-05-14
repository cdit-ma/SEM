#ifndef FILE_H
#define FILE_H
#include "../node.h"

class File : public Node
{
    Q_OBJECT
public:
    File(QString name = "");
    ~File();

    // GraphML interface
public:
    

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};

#endif // FILE_H
