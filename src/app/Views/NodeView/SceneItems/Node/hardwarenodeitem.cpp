#include "hardwarenodeitem.h"

HardwareNodeItem::HardwareNodeItem(NodeViewItem *viewItem, NodeItem *parentItem)
        : StackNodeItem(viewItem, parentItem, Qt::Vertical)
{
    online_icon = {"Icons", "arrowsUpDownDark"};
    offline_icon = {"Notification", "Warning"};

    setIconVisible(EntityRect::SECONDARY_ICON, online_icon, true);
    
    addRequiredData("is_online");
    reloadRequiredData();
}

void HardwareNodeItem::dataChanged(const QString& key_name, const QVariant& data)
{
    if (isDataRequired(key_name)) {
        if (key_name == "is_online") {
            bool is_online = data.toBool();
            setIconVisible(EntityRect::SECONDARY_ICON, is_online ? online_icon : offline_icon, true);
            update();
        }
        StackNodeItem::dataChanged(key_name, data);
    }
}