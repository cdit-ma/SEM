#ifndef DDS_QOSPROFILE_H
#define DDS_QOSPROFILE_H
#include "../qosprofile.h"

class DDS_QOSProfile: public QOSProfile
{
    Q_OBJECT
public:
    DDS_QOSProfile();
    ~DDS_QOSProfile();

    bool canAdoptChild(Node* node);
};
#endif // DDS_QOSPROFILE_H


