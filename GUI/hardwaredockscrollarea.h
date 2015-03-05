#ifndef HARDWAREDOCKSCROLLAREA_H
#define HARDWAREDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class HardwareDockScrollArea : public DockScrollArea
{
    Q_OBJECT
    
public:
    explicit HardwareDockScrollArea(QString label, NodeView *view, DockToggleButton *parent);

    virtual void updateDock();
    
public slots:
    virtual void dockNodeItemClicked();

private:

};

#endif // HARDWAREDOCKSCROLLAREA_H
