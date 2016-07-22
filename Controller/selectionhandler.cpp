#include "selectionhandler.h"
#include "../View/nodeviewitem.h"
#include <QDebug>
int SelectionHandler::_SelectionHandlerID  = 0;
SelectionHandler::SelectionHandler()
{
    ID = ++_SelectionHandlerID;
    currentActiveSelectedItem = 0;
    newActiveSelectedItem = 0;

    //Empty selection will result in destruction.
    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));
}

int SelectionHandler::getID()
{
    return ID;
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

    _selectionChanged(changes);
}

void SelectionHandler::itemDeleted(int ID, ViewItem *item)
{
    int changes = _setItemSelected(item, false, false);
    _selectionChanged(changes);
}

void SelectionHandler::clearSelection()
{
    int changes = _clearSelection();
    _selectionChanged(changes);
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

ViewItem *SelectionHandler::getActiveSelectedItem()
{
    return getFirstSelectedItem();
}

void SelectionHandler::_selectionChanged(int changes = 0)
{
    if(changes > 0){
        emit selectionChanged(currentSelection.size());
    }
    if(newActiveSelectedItem != currentActiveSelectedItem){
        currentActiveSelectedItem = newActiveSelectedItem;
        newActiveSelectedItem = currentActiveSelectedItem;
        emit activeSelectedItemChanged(currentActiveSelectedItem);
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
                //If there is only 1 item there can only be 1 active item.
                if(currentSelection.size() == 1){
                    newActiveSelectedItem = item;
                }
                changeCount += 1;
            }
        }
    }else{
        //Remove it from the map.
        changeCount = currentSelection.removeAll(item);

        //If there is no items left, there is no active item
        if(currentSelection.isEmpty()){
            newActiveSelectedItem = 0;
        }else{
            if(currentActiveSelectedItem == item || newActiveSelectedItem == item){
                newActiveSelectedItem = currentSelection.first();
            }
        }
    }
    if(changeCount > 0 && sendSignal){
        emit itemSelected(item, selected);
    }
    return changeCount;
}

