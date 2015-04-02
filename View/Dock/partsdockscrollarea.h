#ifndef PARTSDOCKSCROLLAREA_H
#define PARTSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class PartsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit PartsDockScrollArea(QString label, NodeView* view, DockToggleButton *parent);

    void addDockNodeItems(QStringList nodeKinds);
    DockNodeItem* getDockNodeItem(QString kind);

public slots:
    virtual void dockNodeItemClicked();
    virtual void updateDock();

private:    
    QStringList parts_notAllowedKinds;
    QStringList displayedItems;

};

#endif // PARTSDOCKSCROLLAREA_H
