#ifndef PARTSDOCKSCROLLAREA_H
#define PARTSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class PartsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit PartsDockScrollArea(DOCK_TYPE type, NodeView* view, DockToggleButton *parent);

    void attachDockSrollArea(DockScrollArea* dock);
    void addDockNodeItems(QStringList nodeKinds);

    DOCK_TYPE kindRequiresDockSwitching(QString dockItemKind);

    bool isDockOpen();

    void connectToView();

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void dockToggled(bool open);
    void attachedDockToggled(bool open);

    void showMainDock();

private:
    void hideDockNodeItems();
    void closeAttachedDocks();

    QHash<DOCK_TYPE, DockScrollArea*> attachedDocks;
    QList<DockScrollArea*> openedDocks;

    QStringList kindsRequiringDefinition;
    QStringList kindsRequiringFunction;
    QStringList displayedItems;

};

#endif // PARTSDOCKSCROLLAREA_H
