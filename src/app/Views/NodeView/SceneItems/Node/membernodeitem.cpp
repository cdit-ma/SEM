#include "membernodeitem.h"

MemberNodeItem::MemberNodeItem(NodeViewItem *viewItem, NodeItem *parentItem)
	: BasicNodeItem(viewItem, parentItem)
{
    addRequiredData(KeyName::Key);
    reloadRequiredData();

    setIcon(EntityRect::MAIN_ICON_OVERLAY, {"Icons", "key"});
}

void MemberNodeItem::dataChanged(const QString& key_name, const QVariant& data)
{
    if (isDataRequired(key_name)) {
        if (key_name == KeyName::Key) {
            bool is_key = data.toBool();
            setIconVisible(EntityRect::MAIN_ICON_OVERLAY, is_key);
        }
        BasicNodeItem::dataChanged(key_name, data);
    }
}