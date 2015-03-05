#ifndef HARDWAREDOCKSCROLLAREA_H
#define HARDWAREDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class HardwareDockScrollArea : public DockScrollArea
{
    Q_OBJECT
    
public:
    explicit HardwareDockScrollArea(QString label, NodeView *view, DockToggleButton *parent);
    
public slots:
    virtual void dockNodeItemClicked();
    virtual void updateDock();

    void nodeConstructed(NodeItem* nodeItem);

private:
    QStringList hardware_notAllowedKinds;

};

#endif // HARDWAREDOCKSCROLLAREA_H
