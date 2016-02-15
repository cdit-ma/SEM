#ifndef DEFINITIONSDOCKSCROLLAREA_H
#define DEFINITIONSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class DefinitionsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:    
    explicit DefinitionsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent = 0);

    QList<DockNodeItem*> getDockNodeItems();
    QList<DockNodeItem*> getDockItemsOfKind(QString nodeKind);

    void connectToView();
    void nodeDeleted(QString nodeID);

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void nodeConstructed(NodeItem* nodeItem);

    void sortDockItems(DockNodeItem* dockItem);
    void sortDockLabelItems(DockNodeItem* dockItem);

    void dockClosed();
    void dockToggled(bool opened);
    void forceOpenDock(QString srcKind);

private:
    void filterDock(QString nodeKind = "");
    void showDockItemsOfKind(QString nodeKind);
    void hideDockItems();

    void hideImplementedComponents();
    void hideSelectedAggregate();
    void showChildrenOutEventPorts();

    void constructDockLabelItem(EntityItem* item);
    void constructDockItem(EntityItem* item);

    QStringList definitions_notAllowedKinds;
    QStringList definitions_allowedKinds;
    QStringList definitionKinds;

    QHash<QString, QVBoxLayout*> idlLayoutItems;
    QHash<QString, QVBoxLayout*> componentLayoutItems;

    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

    int sourceSelectedItemID;
    QString sourceDockItemKind;

};

#endif // DEFINITIONSDOCKSCROLLAREA_H
