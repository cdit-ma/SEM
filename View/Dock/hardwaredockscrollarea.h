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
    void dock_highlightDockItem(NodeItem* = 0);

public slots:
    void dockNodeItemClicked();
    void dockClosed();
    void updateDock();

    void refreshDock();
    void insertDockNodeItem(DockNodeItem* dockItem);

    void nodeConstructed(NodeItem* nodeItem);

private:
    void highlightHardwareConnection(QList<GraphMLItem *> selectedItems);

    QStringList hardware_notAllowedKinds;

};

#endif // HARDWAREDOCKSCROLLAREA_H
