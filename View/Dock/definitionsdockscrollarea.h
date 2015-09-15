#ifndef DEFINITIONSDOCKSCROLLAREA_H
#define DEFINITIONSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class DefinitionsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit DefinitionsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent = 0);

    QList<DockNodeItem*> getDockNodeItems();

    void onNodeDeleted(QString ID);
    void onEdgeDeleted();

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void refreshDock();
    void resortDockItems(DockNodeItem* dockItem);

    void nodeConstructed(NodeItem* nodeItem);

private:
    void hideImplementedComponents();
    void showAllComponents();

    QStringList definitions_notAllowedKinds;
    QHash<QString, QVBoxLayout*> fileLayoutItems;

    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

};

#endif // DEFINITIONSDOCKSCROLLAREA_H
