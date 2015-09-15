#ifndef TOOLBARWIDGETMENU_H
#define TOOLBARWIDGETMENU_H

#include "toolbarwidgetaction.h"
#include <QMenu>

class ToolbarWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ToolbarWidgetMenu(ToolbarWidgetAction* parent_action = 0,
                               ToolbarWidgetAction* default_action = 0,
                               QWidget* parent = 0);


    void setParentAction(ToolbarWidgetAction* widgetAction);
    ToolbarWidgetAction* getParentAction();

    void addWidgetAction(ToolbarWidgetAction* action);
    void removeWidgetAction(ToolbarWidgetAction* action, bool clearing = false);

    QList<ToolbarWidgetAction*> getWidgetActions();
    ToolbarWidgetAction* getWidgetAction(NodeItem* nodeItem);
    bool hasWidgetActions();

    void clearMenu();

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void toolbarMenu_resetActionState();
    void toolbarMenu_hideToolbar(bool triggered);

public slots:
    void close();
    void closeMenu();
    void hideMenu(QAction* action);
    void execMenu();

    void setupDefaultAction();

private:    
    QList<ToolbarWidgetAction*> widgetActions;
    ToolbarWidgetAction* parentAction;
    ToolbarWidgetAction* defaultAction;

    QToolButton* parentButton;
    ToolbarWidgetMenu* parentMenu;

    bool eventFromMenu;
    bool actionTriggered;
};

#endif // TOOLBARWIDGETMENU_H
