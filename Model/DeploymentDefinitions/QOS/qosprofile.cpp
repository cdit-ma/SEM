#include "qosprofile.h"

QOSProfile::QOSProfile(QOS_PROFILE profile):Node(NT_QOSPROFILE)
{
    middleware = profile;
}

QOSProfile::~QOSProfile()
{
}

QOS_PROFILE QOSProfile::getProfileMiddleware()
{
    return middleware;
}

bool QOSProfile::canAdoptChild(Node *node)
{
    return Node::canAdoptChild(node);
}
