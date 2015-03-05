#ifndef PARTSDOCKSCROLLAREA_H
#define PARTSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class PartsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit PartsDockScrollArea(QString label, NodeView* view, DockToggleButton *parent);

    void addDockNodeItems(QStringList nodeKinds);

    virtual void updateDock();

public slots:
    virtual void dockNodeItemClicked();

private:
    QStringList allowedKinds;

};

#endif // PARTSDOCKSCROLLAREA_H
