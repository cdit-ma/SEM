#ifndef PARTSDOCKSCROLLAREA_H
#define PARTSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class PartsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit PartsDockScrollArea(QString label, NodeView* view, DockToggleButton *parent);

    void addDockNodeItems(QStringList nodeKinds);

signals:
    void dock_forceOpenDock(DOCK_TYPE type, QString srcDockItemKind = "");

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void forceOpenDock();

private:    
    QStringList kindsRequiringDefinition;
    QStringList kindsRequiringFunction;
    QStringList displayedItems;

};

#endif // PARTSDOCKSCROLLAREA_H
