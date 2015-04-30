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

    void nodeDeleted(QString childID, QString parentID);

private:    
    QStringList displayedItems;

};

#endif // PARTSDOCKSCROLLAREA_H
