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

    void setProperty(const char *name, const QVariant &value);
    QVariant getProperty(const char *name);

public slots:
    void actionChanged();
    void themeChanged();
    void setChildrenVisible(bool visible);

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    QAction* dockAction;
    int dockActionID;

    QActionGroup* childrenActionGroup;

};

#endif // DOCKWIDGETPARENTACTIONITEM_H
