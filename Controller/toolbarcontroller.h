#ifndef TOOLBARCONTROLLER_H
#define TOOLBARCONTROLLER_H

#include <QToolBar>
#include <QObject>
#include "../View/viewitem.h"
#include "nodeviewitemaction.h"
#include "../Widgets/New/actiongroup.h"
#include "rootaction.h"

class ViewController;
class ToolActionController : public QObject
{
    Q_OBJECT
public:
    ToolActionController(ViewController* viewController);

private slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

    void selectionChanged(int selected);
    void addChildNode();

public:
    QList<QAction*> getAdoptableKindsActions(bool stealth);
    QAction* getAdoptableKindsAction(bool stealth);


    QStringList getKindsRequiringSubActions();
    QList<NodeViewItemAction*> getRequiredSubActionsForKind(QString kind);

    //void connect()
private:
    void setupNodeActions();

    QHash<QString, RootAction*> nodeKindActions;
    QHash<int, NodeViewItemAction*> actions;

public:
    QToolBar* toolbar;

    ActionGroup* actionGroup;
    ViewController* viewController;





    ActionGroup* adoptableKindsGroup;
};

#endif // TOOLBARCONTROLLER_H
