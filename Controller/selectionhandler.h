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
    void setItemSelected(ViewItem* item, bool setSelected, bool append=false);
    void setItemsSelected(QList<ViewItem*> items, bool setSelected, bool append=false);
    void clearSelection();
    QVector<ViewItem*> getSelection();
    int getSelectionCount();
    ViewItem* getFirstSelectedItem();
    ViewItem* getActiveSelectedItem();
signals:
    void itemSelected(ViewItem* item, bool isSelected);
    void activeSelectedItemChanged(ViewItem* item);
    void selectionChanged(int count);
private slots:
    void itemDeleted(int ID, ViewItem *item);
private:
    void _selectionChanged(int changes);
    int _clearSelection();
    int _setItemSelected(ViewItem* item, bool selected, bool sendSignal=true);

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
