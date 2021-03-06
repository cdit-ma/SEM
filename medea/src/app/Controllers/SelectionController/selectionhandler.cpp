#include "selectionhandler.h"

int SelectionHandler::_SelectionHandlerID  = 0;

SelectionHandler::SelectionHandler()
{
    ID = ++_SelectionHandlerID;

    //Empty selection will result in destruction.
    connect(this, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(deleteLater()));
}

int SelectionHandler::getID()
{
    return ID;
}

void SelectionHandler::toggleItemsSelection(ViewItem *item, bool append)
{
    QList<ViewItem*> items;
    items << item;
    toggleItemsSelection(items, append);
}

void SelectionHandler::toggleItemsSelection(const QList<ViewItem*>& items, bool append)
{
    int changes = 0;
    if (!append) {
        // Deselect previous selection first if we're not appending to the current list of selection
        changes += _clearSelection();
    }
    for (ViewItem* item : items) {
        changes += _toggleItemsSelection(item);
    }
    _selectionChanged(changes);
}

void SelectionHandler::itemDeleted(int id, ViewItem *item)
{
    Q_UNUSED(id)
    int changes = _toggleItemsSelection(item, true);
    _selectionChanged(changes);
}

void SelectionHandler::clearSelection()
{
    int changes = _clearSelection();
    _selectionChanged(changes);
}

void SelectionHandler::setActiveSelectedItem(ViewItem *viewItem)
{
    if(viewItem && currentSelection.contains(viewItem)){
        newActiveSelectedItem = viewItem;
        _selectionChanged();
    }
}

void SelectionHandler::cycleActiveSelectedItem(bool forward)
{
    if(currentActiveSelectedItem){
        int index = currentSelection.indexOf(currentActiveSelectedItem);
        int lastPos = currentSelection.size() - 1;
        if(forward){
            index ++;
        }else{
            index --;
        }
        if(index > lastPos){
            index = 0;
        }else if(index < 0){
            index = lastPos;
        }
        newActiveSelectedItem = currentSelection.at(index);
        _selectionChanged();
    }
}

QVector<ViewItem *> SelectionHandler::getSelection() const
{
    return currentSelection;
}

int SelectionHandler::getSelectionCount()
{
    return currentSelection.count();
}

ViewItem *SelectionHandler::getFirstSelectedItem()
{
    ViewItem* item = nullptr;
    if(!currentSelection.isEmpty()){
        item = currentSelection.first();
    }
    return item;
}

ViewItem *SelectionHandler::getActiveSelectedItem()
{
    return newActiveSelectedItem;
}

void SelectionHandler::_selectionChanged(int changes)
{
    if(changes > 0){
        emit selectionChanged(currentSelection.size());
    }
    if(newActiveSelectedItem != currentActiveSelectedItem){
        if(currentActiveSelectedItem){
            emit itemActiveSelectionChanged(currentActiveSelectedItem, false);
        }
        currentActiveSelectedItem = newActiveSelectedItem;
        newActiveSelectedItem = currentActiveSelectedItem;

        emit itemActiveSelectionChanged(currentActiveSelectedItem, true);
    }
}

int SelectionHandler::_clearSelection()
{
    int itemsChanged = 0;
    auto current_selection = currentSelection;
    for (ViewItem* item : current_selection) {
        itemsChanged += _toggleItemsSelection(item);
    }
    return itemsChanged;
}

int SelectionHandler::_toggleItemsSelection(ViewItem *item, bool deletingItem)
{
    int changeCount = 0;
    bool inSelection = currentSelection.contains(item);

    if (deletingItem) {
        if (!inSelection) {
            // We don't need to unselect item.
            return changeCount;
        }
    }
    changeCount += _setItemSelected(item, !inSelection);

    if (changeCount > 0 && !deletingItem) {
        emit itemSelectionChanged(item, !inSelection);
    }
    return changeCount;
}

int SelectionHandler::_setItemSelected(ViewItem *item, bool selected)
{
    int changeCount = 0;
    if(selected){
        //Register the selection handler
        item->registerObject(this);
        currentSelection.append(item);
        //If there is only 1 item there can only be 1 active item.
        if(currentSelection.size() == 1){
            newActiveSelectedItem = item;
        }
        changeCount += 1;
    }else{
        //Remove it from the map.
        changeCount = currentSelection.removeAll(item);
        if(changeCount > 0){
            //Unregister the selection handler
            item->unregisterObject(this);
        }
        //If there is no items left, there is no active item
        if(currentSelection.isEmpty()){
            newActiveSelectedItem = nullptr;
        }else{
            if(currentActiveSelectedItem == item || newActiveSelectedItem == item){
                newActiveSelectedItem = currentSelection.first();
            }
        }
    }
    return changeCount;
}