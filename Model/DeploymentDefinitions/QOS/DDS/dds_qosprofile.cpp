#include "dds_qosprofile.h"

DDS_QOSProfile::DDS_QOSProfile():QOSProfile(QP_DDS)
{
}

DDS_QOSProfile::~DDS_QOSProfile()
{

}


bool DDS_QOSProfile::canAdoptChild(Node *node)
{
    if(node->getNodeType() != NT_QOS){
        return false;
    }

    QString nodeKind = node->getNodeKind();
    //ONE QOS OF EACH KIND.
    if(!getChildrenOfKind(nodeKind).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(node);
}
