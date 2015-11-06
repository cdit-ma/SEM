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

public slots:
    void dockNodeItemClicked();
    void updateDock();
    void clear();

    void insertDockNodeItem(DockNodeItem* dockItem);

    void dockClosed();

private:
    QHash<QString, QVBoxLayout*> classLayoutItems;
    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

    QStringList allowedKinds;
};

#endif // FUNCTIONSDOCKSCROLLAREA_H
