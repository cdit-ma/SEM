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

public slots:
    virtual void dockNodeItemClicked();
    virtual void updateDock();

    void nodeConstructed(NodeItem* nodeItem);

    void clickHardwareDockItem(Node* component, NodeItem* hardwareNodeItem);

private:
    QStringList hardware_notAllowedKinds;
    Node* component;

};

#endif // HARDWAREDOCKSCROLLAREA_H
