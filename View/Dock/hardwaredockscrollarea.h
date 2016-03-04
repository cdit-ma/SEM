#ifndef HARDWAREDOCKSCROLLAREA_H
#define HARDWAREDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class HardwareDockScrollArea : public DockScrollArea
{
    Q_OBJECT
    
public:
    explicit HardwareDockScrollArea(DOCK_TYPE type, NodeView *view, DockToggleButton *parent);

    void connectToView();
    
signals:
    void dock_highlightDockItem(NodeItem* nodeItem = 0);

public slots:
    void dockNodeItemClicked();
    void updateDock();

    void nodeConstructed(NodeItem* nodeItem);
    void insertDockNodeItem(DockNodeItem* dockItem);

    void displayHighlightedItem();

private:
    void highlightHardwareConnection(QList<GraphMLItem*> selectedItems);

    QStringList hardware_notAllowedKinds;

};

#endif // HARDWAREDOCKSCROLLAREA_H
