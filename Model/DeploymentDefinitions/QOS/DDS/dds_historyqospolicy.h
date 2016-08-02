#ifndef DDS_HISTORYQOSPOLICY_H
#define DDS_HISTORYQOSPOLICY_H
#include "../../../node.h"

class DDS_HistoryQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_HistoryQosPolicy();
    ~DDS_HistoryQosPolicy();

    bool canAdoptChild(Node* node);
};
#endif // DDS_HISTORYQOSPOLICY_H


