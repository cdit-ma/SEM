#ifndef DEFINITIONSDOCKSCROLLAREA_H
#define DEFINITIONSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class DefinitionsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:    
    explicit DefinitionsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent = 0);

    QList<DockNodeItem*> getDockNodeItems();

    void nodeDeleted(QString nodeID);

signals:
    void dock_forceOpenDock(DOCK_TYPE type);

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void nodeConstructed(NodeItem* nodeItem);
    void insertDockNodeItem(DockNodeItem* dockItem);

    void dockClosed();
    void forceOpenDock(QString srcKind);

    // these are currently only being used for Vector items
    void showDockItem(int nodeID);
    void hideDockItem(int nodeID);

private:
    void filterDock(QString nodeKind = "");
    void showDockItemsOfKind(QString nodeKind);
    void hideImplementedComponents();

    QStringList definitions_notAllowedKinds;
    QStringList definitions_allowedKinds;
    QStringList definitionKinds;

    QHash<QString, QVBoxLayout*> fileLayoutItems;
    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

    int sourceSelectedItemID;
    QString sourceDockItemKind;

};

#endif // DEFINITIONSDOCKSCROLLAREA_H
