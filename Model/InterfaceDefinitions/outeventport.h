#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "../node.h"


class OutEventPort : public Node
{
public:
    OutEventPort(QString name="");
    ~OutEventPort();

    QString toString();

    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
private:
};

#endif // OUTEVENTPORT_H
