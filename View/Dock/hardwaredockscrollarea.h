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
    void dock_higlightDockItem(Node* node);

public slots:
    virtual void dockNodeItemClicked();
    virtual void updateDock();

    void nodeConstructed(NodeItem* nodeItem);

private:
    Edge* getHardwareConnection(Node* selectedNode);

    QStringList hardware_notAllowedKinds;

};

#endif // HARDWAREDOCKSCROLLAREA_H
