#ifndef TOOLBARWIDGETMENU_H
#define TOOLBARWIDGETMENU_H

#include "toolbarwidgetaction.h"

#include <QMenu>

class ToolbarWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ToolbarWidgetMenu(ToolbarWidgetAction* parent_action = 0,
                               ToolbarWidgetAction *default_action = 0,
                               QWidget *parent = 0);

    void clearMenu();

    void addWidgetAction(ToolbarWidgetAction* action);
    void removeWidgetAction(ToolbarWidgetAction* action);

    QList<ToolbarWidgetAction*> getWidgetActions();
    ToolbarWidgetAction* getWidgetAction(NodeItem* nodeItem);

    void setParentAction(ToolbarWidgetAction* widgetAction);
    ToolbarWidgetAction* getParentAction();

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void toolbarMenu_resetActionState();
    void toolbarMenu_hideToolbar(bool triggered);

    void toolbarMenu_closeParentMenu();
    void toolbarMenu_parentTriggered(bool triggered);

public slots:
    void close();
    void closeMenu();
    void hideMenu(QAction* action);
    void execMenu();

    void setupDefaultAction();

private:    
    ToolbarWidgetAction* parentAction;
    QList<ToolbarWidgetAction*> widgetActions;

    bool eventFromMenu;
    bool actionTriggered;

    ToolbarWidgetAction* defaultAction;

};

#endif // TOOLBARWIDGETMENU_H
