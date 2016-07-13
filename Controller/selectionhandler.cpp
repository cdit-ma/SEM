#include "selectionhandler.h"
#include "../View/nodeviewitem.h"

SelectionHandler::SelectionHandler()
{

}

void SelectionHandler::setItemSelected(ViewItem *item, bool setSelected, bool append)
{
    QList<ViewItem*> items;
    items << item;
    setItemsSelected(items, setSelected, append);
}

void SelectionHandler::setItemsSelected(QList<ViewItem *> items, bool setSelected, bool append)
{
    int changes = 0;
    if(!append){
        //Unselect for non-append
        changes += _clearSelection();
    }
    foreach(ViewItem* item, items){
        changes += _setItemSelected(item, setSelected);
    }

    if(changes > 0){
        _selectionChanged();
    }
}

void SelectionHandler::itemDeleted(ViewItem *item)
{
    if(_setItemSelected(item, false, false) > 0){
        _selectionChanged();
    }
}

void SelectionHandler::clearSelection()
{
    if(_clearSelection() > 0){
        _selectionChanged();
    }
}

QVector<ViewItem *> SelectionHandler::getSelection()
{
    return currentSelection;
}

int SelectionHandler::getSelectionCount()
{
    return currentSelection.count();
}

ViewItem *SelectionHandler::getFirstSelectedItem()
{
    ViewItem* item = 0;
    if(!currentSelection.isEmpty()){
        item = currentSelection.first();
    }
    return item;
}

void SelectionHandler::_selectionChanged()
{
    emit selectionChanged(currentSelection.size());
}

bool SelectionHandler::appendToSelection(ViewItem *item)
{
    if(!item || isItemsAncestorSelected(item)){
        return false;
    }

    //Unset Items Descendants.
    int itemsChanged = unsetItemsDescendants(item);

    itemsChanged += _setItemSelected(item, true);

    if(itemsChanged > 0){
        emit selectionChanged(itemsChanged);
    }
}

int SelectionHandler::_clearSelection()
{
    int itemsChanged = 0;
    foreach(ViewItem* item, currentSelection){
        itemsChanged += _setItemSelected(item, false);
    }
    return itemsChanged;
}

/**
 * @brief SelectionHandler::isItemsAncestorSelected Returns whether or not an Item's ancestor is already selected.
 * @param item
 * @return
 */
bool SelectionHandler::isItemsAncestorSelected(ViewItem *item)
{
    bool selected = false;
    if(item->isNode()){
        NodeViewItem* nodeItem = (NodeViewItem*)item;
        foreach(ViewItem* selectedItem, currentSelection){
            if(selectedItem->isNode()){
                NodeViewItem* selectedNodeItem = (NodeViewItem*)selectedItem;
                if(selectedNodeItem->isAncestorOf(nodeItem)){
                    selected = true;
                    break;
                }
            }
        }
    }
    return selected;
}

int SelectionHandler::unsetItemsDescendants(ViewItem *item)
{
    int itemsUnset = 0;
    if(item->isNode()){
        NodeViewItem* nodeItem = (NodeViewItem*)item;
        foreach(ViewItem* selectedItem, currentSelection){
            if(selectedItem->isNode()){
                NodeViewItem* selectedNodeItem = (NodeViewItem*)selectedItem;

                if(nodeItem->isAncestorOf(selectedNodeItem)){
                    //Remove it.
                    itemsUnset += _setItemSelected(selectedItem, false);
                }
            }
        }
    }
    return itemsUnset;
}

int SelectionHandler::_setItemSelected(ViewItem *item, bool selected, bool sendSignal)
{
    int changeCount = 0;
    if(selected){
        if(!currentSelection.contains(item)){
            if(!isItemsAncestorSelected(item)){
                changeCount += unsetItemsDescendants(item);
                currentSelection.append(item);
                changeCount += 1;
            }
        }
    }else{
        //Remove it from the map.
        changeCount = currentSelection.removeAll(item);
    }
    if(changeCount > 0 && sendSignal){
        emit itemSelected(item, selected);
    }
    return changeCount;
}

