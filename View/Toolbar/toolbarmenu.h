#ifndef TOOLBARMENU_H
#define TOOLBARMENU_H

#include "toolbarmenuaction.h"
#include <QMenu>

class ToolbarMenu : public QMenu
{
    Q_OBJECT

public:
    explicit ToolbarMenu(ToolbarWidget* toolbar, ToolbarMenuAction* info_action = 0);

    void addAction(ToolbarMenuAction* action);
    void removeAction(ToolbarMenuAction* action, bool clearing = false);

    ToolbarMenuAction* getAction(NodeItem* item);
    QList<ToolbarMenuAction*> getActions(NodeItem* item = 0);

    bool isEmpty();
    void clearMenu();

    //void moveActions();
    void splitActionsWithMenu();

signals:
    void toolbarMenu_triggered(ToolbarMenuAction* action);
    void toolbarMenu_hoverLeave(QAction* action = 0);

private slots:
    void menuTriggered(QAction* action);

protected:
    void mousePressEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);

private:
    void setupInfoAction();
    void insertMenuAction(ToolbarMenuAction* action);

    ToolbarMenuAction* infoAction;
    QList<ToolbarMenuAction*> menuActions;

};

#endif // TOOLBARMENU_H
