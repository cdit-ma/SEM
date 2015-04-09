#ifndef DEFINITIONSDOCKSCROLLAREA_H
#define DEFINITIONSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class DefinitionsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit DefinitionsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent = 0);

    QList<DockNodeItem*> getDockNodeItems();

public slots:
    virtual void dockNodeItemClicked();
    virtual void updateDock();

    void nodeConstructed(NodeItem* nodeItem);
    void nodeDestructed(NodeItem* nodeItem);

    void resortDockItems(DockNodeItem* dockItem);

private:
    QStringList definitions_notAllowedKinds;
    QHash<NodeItem*, QVBoxLayout*> fileLayoutItems;

    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

};

#endif // DEFINITIONSDOCKSCROLLAREA_H
