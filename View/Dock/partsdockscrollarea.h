#ifndef PARTSDOCKSCROLLAREA_H
#define PARTSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class PartsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit PartsDockScrollArea(DOCK_TYPE type, NodeView* view, DockToggleButton *parent);

    void addDockNodeItems(QStringList nodeKinds);
    bool kindRequiresDockSwitching(QString dockItemKind);

    void connectToView();

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void forceOpenDock();

private:
    void hideDockNodeItems();

    QStringList kindsRequiringDefinition;
    QStringList kindsRequiringFunction;
    QStringList displayedItems;

};

#endif // PARTSDOCKSCROLLAREA_H
