#ifndef FUNCTIONSDOCKSCROLLAREA_H
#define FUNCTIONSDOCKSCROLLAREA_H

#include "dockscrollarea.h"
#include <QPair>

class FunctionsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit FunctionsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent = 0);

    void addDockNodeItems(QList<QPair<QString, QString> > functionKinds);

signals:
    void dock_forceOpenDock(DOCK_TYPE type);

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void insertDockNodeItem(DockNodeItem* dockItem);

    void dockClosed();
    void forceOpenDock();

private:
    QHash<QString, QVBoxLayout*> classLayoutItems;
    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

    QStringList functions_allowedKinds;
};

#endif // FUNCTIONSDOCKSCROLLAREA_H
