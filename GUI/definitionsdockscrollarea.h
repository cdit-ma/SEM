#ifndef DEFINITIONSDOCKSCROLLAREA_H
#define DEFINITIONSDOCKSCROLLAREA_H

#include "dockscrollarea.h"

class DefinitionsDockScrollArea : public DockScrollArea
{
    Q_OBJECT

public:
    explicit DefinitionsDockScrollArea(QString label, NodeView *view, DockToggleButton *parent = 0);

public slots:
    virtual void dockNodeItemClicked();
    virtual void updateDock();

    void nodeConstructed(NodeItem* nodeItem);

private:
    QStringList definitions_notAllowedKinds;

};

#endif // DEFINITIONSDOCKSCROLLAREA_H
