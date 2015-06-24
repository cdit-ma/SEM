#ifndef HARDWAREDOCKSCROLLAREA_H
#define HARDWAREDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class HardwareDockScrollArea : public DockScrollArea
{
    Q_OBJECT
    
public:
    explicit HardwareDockScrollArea(QString label, NodeView *view, DockToggleButton *parent);
    
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
    void onEdgeDeleted();
};

#endif // HARDWAREDOCKSCROLLAREA_H
