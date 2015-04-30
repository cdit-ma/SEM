#ifndef HARDWAREDOCKSCROLLAREA_H
#define HARDWAREDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class HardwareDockScrollArea : public DockScrollArea
{
    Q_OBJECT
    
public:
    explicit HardwareDockScrollArea(QString label, NodeView *view, DockToggleButton *parent);
    
signals:
    void dock_destructEdge(Edge* edge);
    void dock_higlightDockItem(Node* node = 0);

public slots:
    virtual void dockNodeItemClicked();
    virtual void updateDock();
    void edgeDeleted(QString srcID, QString dstID);

    void insertDockNodeItem(DockNodeItem* dockItem);

    void nodeConstructed(NodeItem* nodeItem);
    void refreshDock();

private:
    Edge* getHardwareConnection(Node* selectedNode);
    void highlightHardwareConnection();

    QStringList hardware_notAllowedKinds;

};

#endif // HARDWAREDOCKSCROLLAREA_H
