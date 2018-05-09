#ifndef DDS_QOSPROFILE_H
#define DDS_QOSPROFILE_H
#include "../../../node.h"

class EntityFactory;
class DDS_QOSProfile: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_QOSProfile(EntityFactory& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* node);
};
#endif // DDS_QOSPROFILE_H


