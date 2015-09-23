#ifndef TOOLBARWIDGETMENU_H
#define TOOLBARWIDGETMENU_H

#include "toolbarwidgetaction.h"
#include <QMenu>

class ToolbarWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ToolbarWidgetMenu(ToolbarWidget* toolbar,
                               ToolbarWidgetAction* parent_action = 0,
                               QWidget* parent = 0);

    void setDefaultAction(ToolbarWidgetAction* action);

    void setParentAction(ToolbarWidgetAction* action);
    ToolbarWidgetAction* getParentAction();

    void addWidgetAction(ToolbarWidgetAction* action);
    void removeWidgetAction(ToolbarWidgetAction* action, bool clearing = false);

    QList<ToolbarWidgetAction*> getWidgetActions();
    ToolbarWidgetAction* getWidgetAction(NodeItem* nodeItem);
    bool hasWidgetActions();

    bool isOpen();
    void clearMenu();

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void toolbarMenu_hasFocus(ToolbarWidgetMenu* menu);
    void toolbarMenu_setFocus(bool hasFocus);
    void toolbarMenu_hideToolbar(bool triggered);
    void toolbarMenu_resetParentAction();

public slots:
    void close();
    void closeMenu();
    void hideMenu(QAction* action);
    void execMenu();

    void menuOpened();
    void setFocus(ToolbarWidgetMenu* menu);

private:    
    void setupDefaultAction();

    QList<ToolbarWidgetAction*> widgetActions;
    ToolbarWidgetAction* parentAction;
    ToolbarWidgetAction* defaultAction;

    QToolButton* parentButton;
    ToolbarWidgetMenu* parentMenu;

    bool open;
    bool hasFocus;
    bool eventFromMenu;
    bool actionTriggered;
};

#endif // TOOLBARWIDGETMENU_H
