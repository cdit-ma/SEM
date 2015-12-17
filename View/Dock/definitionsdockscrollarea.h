#ifndef DEFINITIONSDOCKSCROLLAREA_H
#define DEFINITIONSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class DefinitionsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:    
    explicit DefinitionsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent = 0);

    QList<DockNodeItem*> getDockNodeItems();

    void connectToView();
    void nodeDeleted(QString nodeID);

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void nodeConstructed(NodeItem* nodeItem);
    void insertDockNodeItem(DockNodeItem* dockItem);

    void dockClosed();
    void forceOpenDock(QString srcKind);

private:
    void filterDock(QString nodeKind = "");
    void showDockItemsOfKind(QString nodeKind);
    void hideDockItems();

    void hideImplementedComponents();
    void showChildrenOutEventPorts();

    void sortDockItems(DockNodeItem* dockItem);
    void sortDockLabelItems(DockNodeItem* dockItem);

    void constructLabelDockItem(EntityItem* item);
    void constructDockItem(EntityItem* item);

    QStringList definitions_notAllowedKinds;
    QStringList definitions_allowedKinds;
    QStringList definitionKinds;

    QHash<QString, QVBoxLayout*> idlLayoutItems;
    QHash<QString, QVBoxLayout*> componentLayoutItems;
    QList<DockNodeItem*> tempDockItems;

    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

    int sourceSelectedItemID;
    QString sourceDockItemKind;

};

#endif // DEFINITIONSDOCKSCROLLAREA_H
