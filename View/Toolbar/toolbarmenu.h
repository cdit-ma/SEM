#ifndef TOOLBARMENU_H
#define TOOLBARMENU_H

#include "toolbarmenuaction.h"
#include <QMenu>

class ToolbarMenu : public QMenu
{
    Q_OBJECT

public:
    explicit ToolbarMenu(QWidget* parent = 0, ToolbarMenuAction* info_action = 0);

    void addAction(ToolbarMenuAction* action);
    void removeAction(ToolbarMenuAction* action, bool clearing = false);
    ToolbarMenuAction* getAction(NodeItem* item);

    bool isEmpty();
    void clearMenu();

signals:
    void toolbarMenu_triggered(ToolbarMenuAction* action);

private slots:
    void menuTriggered(QAction* action);

private:
    void setupInfoAction();

    ToolbarMenuAction* infoAction;
    QList<ToolbarMenuAction*> menuActions;

};

#endif // TOOLBARMENU_H
