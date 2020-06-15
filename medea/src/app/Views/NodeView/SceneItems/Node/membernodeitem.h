#ifndef MEMBER_NODEITEM_H
#define MEMBER_NODEITEM_H

#include "basicnodeitem.h"

class MemberNodeItem: public BasicNodeItem
{
    Q_OBJECT
    
public:
    MemberNodeItem(NodeViewItem *viewItem, NodeItem *parentItem);
    
protected:
    void dataChanged(const QString& key_name, const QVariant& data) override;
};

#endif // MEMBER_NODEITEM_H
