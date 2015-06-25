#ifndef HARDWAREDOCKSCROLLAREA_H
#define HARDWAREDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class HardwareDockScrollArea : public DockScrollArea
{
    Q_OBJECT
    
public:
    explicit HardwareDockScrollArea(QString label, NodeView *view, DockToggleButton *parent);

    void onNodeDeleted(QString ID);
    void onEdgeDeleted();
    
signals:
    void dock_higlightDockItem(NodeItem* = 0);
public slots:
    void dockNodeItemClicked();
    void updateDock();

    void insertDockNodeItem(DockNodeItem* dockItem);
    void removeDockNodeItem(QString ID);
    void refreshDock();

    void nodeConstructed(NodeItem* nodeItem);

private:
    void highlightHardwareConnection();

    QStringList hardware_notAllowedKinds;


    // DockScrollArea interface
public:
};

#endif // HARDWAREDOCKSCROLLAREA_H
