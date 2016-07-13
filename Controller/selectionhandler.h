#ifndef SELECTIONHANDLER_H
#define SELECTIONHANDLER_H

#include <QObject>

#include "../View/viewitem.h"

class SelectionHandler: public QObject
{
    Q_OBJECT
public:
    SelectionHandler();

    void setItemSelected(ViewItem* item, bool setSelected, bool append=false);
    void setItemsSelected(QList<ViewItem*> items, bool setSelected, bool append=false);
    void itemDeleted(ViewItem* item);
    void clearSelection();
    QVector<ViewItem*> getSelection();
    int getSelectionCount();
    ViewItem* getFirstSelectedItem();
signals:
    void itemSelected(ViewItem* item, bool isSelected);
    void itemActiveSelected(ViewItem* item);
    void selectionChanged(int count);
private:
    void _selectionChanged();
    bool appendToSelection(ViewItem* item);
    int _clearSelection();
    bool isItemsAncestorSelected(ViewItem* item);

    int unsetItemsDescendants(ViewItem* item);

    int _setItemSelected(ViewItem* item, bool selected, bool sendSignal=true);

private:
    QVector<ViewItem*> currentSelection;
};

#endif // SELECTIONHANDLER_H
