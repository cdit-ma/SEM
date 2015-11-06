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
    void edgeDeleted();

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void nodeConstructed(NodeItem* nodeItem);
    void insertDockNodeItem(DockNodeItem* dockItem);
    void refreshDock();

private:
    void showDockItemsOfKind(QString kind = "");
    void hideImplementedComponents();
    void hideBlackBoxes();

    QStringList definitions_notAllowedKinds;
    QHash<QString, QVBoxLayout*> fileLayoutItems;

    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

};

#endif // DEFINITIONSDOCKSCROLLAREA_H
