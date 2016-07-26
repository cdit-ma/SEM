#ifndef SELECTIONHANDLER_H
#define SELECTIONHANDLER_H

#include <QObject>

#include "../Controller/qobjectregistrar.h"
#include "../View/viewitem.h"

class SelectionController;
class SelectionHandler: public QObjectRegistrar
{
    friend class SelectionController;
    Q_OBJECT
protected:
    SelectionHandler();
    int getID();

public:
    void toggleItemsSelection(ViewItem* item, bool append = false);
    void toggleItemsSelection(QList<ViewItem*> items, bool append = false);

    void clearSelection();

    void setActiveSelectedItem(ViewItem* viewItem);
    void cycleActiveSelectedItem(bool forward = true);

    QVector<ViewItem*> getSelection();
    int getSelectionCount();

    ViewItem* getFirstSelectedItem();
    ViewItem* getActiveSelectedItem();
signals:
    void itemSelectionChanged(ViewItem*, bool selected);
    void itemActiveSelectionChanged(ViewItem* item, bool active);

    void selectionChanged(int count);
private slots:
    void itemDeleted(int ID, ViewItem *item);
private:
    void _selectionChanged(int changes = 0);
    int _clearSelection();
    int _toggleItemsSelection(ViewItem* item, bool deletingItem=false);
    int _setItemSelected(ViewItem* item, bool selected);



    bool isItemsAncestorSelected(ViewItem* item);
    int unsetItemsDescendants(ViewItem* item);

    int ID;
private:
    static int _SelectionHandlerID;
    ViewItem* currentActiveSelectedItem;
    ViewItem* newActiveSelectedItem;
    QVector<ViewItem*> currentSelection;
};

#endif // SELECTIONHANDLER_H
