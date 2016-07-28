#ifndef TOOLBARCONTROLLER_H
#define TOOLBARCONTROLLER_H

#include <QToolBar>
#include <QObject>
#include "../View/viewitem.h"
#include "nodeviewitemaction.h"
#include "../Widgets/New/actiongroup.h"

class ViewController;
class ToolbarController : public QObject
{
    Q_OBJECT
public:
    ToolbarController(ViewController* viewController);

private slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

private:
    QHash<int, NodeViewItemAction*> actions;

public:
    QToolBar* toolbar;
    ActionGroup* actionGroup;
};

#endif // TOOLBARCONTROLLER_H
