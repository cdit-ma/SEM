#ifndef SELECTIONHANDLER_H
#define SELECTIONHANDLER_H

#include <QObject>

#include "../../Utils/qobjectregistrar.h"
#include "../ViewController/viewitem.h"

class SelectionController;
class SelectionHandler: public QObjectRegistrar
{
    friend class SelectionController;
    Q_OBJECT
    
protected:
    explicit SelectionHandler();
    int getID();

public:
    void toggleItemsSelection(ViewItem* item, bool append = false);
    void toggleItemsSelection(const QList<ViewItem*>& items, bool append = false);

    void setActiveSelectedItem(ViewItem* viewItem);
    void cycleActiveSelectedItem(bool forward = true);

    QVector<ViewItem*> getSelection() const;
    int getSelectionCount();

    ViewItem* getFirstSelectedItem();
    ViewItem* getActiveSelectedItem();

signals:
    void itemSelectionChanged(ViewItem*, bool selected);
    void itemActiveSelectionChanged(ViewItem* item, bool active);
    void selectionChanged(int count);
    
public slots:
    void clearSelection();
    void itemDeleted(int id, ViewItem *item);
    
private:
    void _selectionChanged(int changes = 0);
    int _clearSelection();
    int _toggleItemsSelection(ViewItem* item, bool deletingItem=false);
    int _setItemSelected(ViewItem* item, bool selected);

    int ID;
    
    static int _SelectionHandlerID;
    
    ViewItem* currentActiveSelectedItem = nullptr;
    ViewItem* newActiveSelectedItem = nullptr;
    
    QVector<ViewItem*> currentSelection;
};

#endif // SELECTIONHANDLER_H
