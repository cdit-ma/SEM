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
    ToolbarWidgetAction* getParentAction();

protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);

signals:
    void hideToolbar();
    void closeParentMenu();
    void connectToParentMenu(ToolbarWidgetMenu* menu);

public slots:
    void close();
    void closeMenu();
    void hideMenu(QAction* action);
    void execMenu();

    void setupDefaultAction();
    void connectChildMenu(ToolbarWidgetMenu* menu);

private:
    ToolbarWidgetAction* parentAction;
    QList<ToolbarWidgetAction*> widgetActions;
    bool eventFromMenu;

    ToolbarWidgetAction* defaultAction;

};

#endif // TOOLBARWIDGETMENU_H
