#ifndef TOOLBARWIDGETMENU_H
#define TOOLBARWIDGETMENU_H

#include "toolbarwidgetaction.h"

#include <QMenu>

class ToolbarWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ToolbarWidgetMenu(ToolbarWidgetAction* widgetAction = 0,
                               ToolbarWidgetAction *action = 0,
                               QWidget *parent = 0);

    void clearMenu();

    void addWidgetAction(ToolbarWidgetAction* action);
    QList<ToolbarWidgetAction*> getWidgetActions();
    ToolbarWidgetAction* getWidgetAction(Node* node);

    void setParentAction(ToolbarWidgetAction* widgetAction);
    ToolbarWidgetAction* getParentAction();

protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);

signals:
    void toolbarMenu_connectToParentMenu(ToolbarWidgetMenu* menu);
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
    void connectChildMenu(ToolbarWidgetMenu* menu);
    void setParentTriggered(bool triggered);

private:    
    ToolbarWidgetAction* parentAction;
    QList<ToolbarWidgetAction*> widgetActions;

    bool eventFromMenu;
    bool actionTriggered;

    ToolbarWidgetAction* defaultAction;

};

#endif // TOOLBARWIDGETMENU_H
