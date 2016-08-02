#ifndef QOSPROFILE_H
#define QOSPROFILE_H
#include "../../node.h"

enum QOS_PROFILE{QP_DDS};
class QOSProfile: public Node
{
    Q_OBJECT
public:
    QOSProfile(QOS_PROFILE profile);
    ~QOSProfile();

    QOS_PROFILE getProfileMiddleware();

    bool canAdoptChild(Node* node);
private:
    QOS_PROFILE middleware;
};
#endif // QOSProfile_H


