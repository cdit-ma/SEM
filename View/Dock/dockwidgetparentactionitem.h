#ifndef DOCKWIDGETPARENTACTIONITEM_H
#define DOCKWIDGETPARENTACTIONITEM_H

#include "dockwidgetitem.h"
#include <QActionGroup>

class DockWidgetParentActionItem : public DockWidgetItem
{
    Q_OBJECT

public:
    DockWidgetParentActionItem(QAction* action, QWidget* parent = 0);

    QAction* getAction();

    void addToChildrenActions(QAction* action);
    void setChildrenActions(QList<QAction*> actions);

    void setToggledState(bool toggled);
    DOCKITEM_KIND getItemKind();

public slots:
    void actionChanged();
    void themeChanged();
    void setChildrenVisible(bool visible);

private:
    QAction* dockAction;
    QActionGroup* childrenActionGroup;

};

#endif // DOCKWIDGETPARENTACTIONITEM_H
