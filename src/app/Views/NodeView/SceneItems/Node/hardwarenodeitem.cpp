#include "hardwarenodeitem.h"

HardwareNodeItem::HardwareNodeItem(NodeViewItem *viewItem, NodeItem *parentItem):
    StackNodeItem(viewItem, parentItem, Qt::Vertical)
{
    online_icon = {"Icons", "arrowsUpDownDark"};
    offline_icon = {"Notification", "Warning"};
    
    addRequiredData("is_online");
    reloadRequiredData();
}

void HardwareNodeItem::dataChanged(QString keyName, QVariant data)
{
    if(getRequiredDataKeys().contains(keyName)){
        if(keyName == "is_online"){
            bool is_online = data.toBool();
            setSecondaryIconPath(is_online ? online_icon : offline_icon);
            update();
        }
        StackNodeItem::dataChanged(keyName, data);
    }
}