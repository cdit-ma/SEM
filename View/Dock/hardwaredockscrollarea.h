#ifndef HARDWAREDOCKSCROLLAREA_H
#define HARDWAREDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class HardwareDockScrollArea : public DockScrollArea
{
    Q_OBJECT
    
public:
    explicit HardwareDockScrollArea(QString label, NodeView *view, DockToggleButton *parent);

    void onEdgeDeleted();
    
signals:
    void dock_higlightDockItem(GraphMLItem* = 0);

public slots:
    void dockNodeItemClicked();
    void dockClosed();
    void updateDock();

    void refreshDock();
    void insertDockNodeItem(DockNodeItem* dockItem);

    void nodeConstructed(EntityItem* nodeItem);

private:
    void highlightHardwareConnection(QList<EntityItem *> selectedItems);

    QStringList hardware_notAllowedKinds;

};

#endif // HARDWAREDOCKSCROLLAREA_H
