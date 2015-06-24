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
    void dock_higlightDockItem(Node* node = 0);

public slots:
    void dockNodeItemClicked();
    void updateDock();

    void insertDockNodeItem(DockNodeItem* dockItem);
    void refreshDock();

    void nodeConstructed(NodeItem* nodeItem);

private:
    Edge* getHardwareConnection(Node* selectedNode);
    void highlightHardwareConnection();

    QStringList hardware_notAllowedKinds;


    // DockScrollArea interface
public:
};

#endif // HARDWAREDOCKSCROLLAREA_H
