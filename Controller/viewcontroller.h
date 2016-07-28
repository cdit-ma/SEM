#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "entityadapter.h"
#include "../View/viewitem.h"
#include "selectionhandler.h"
#include "../Widgets/New/selectioncontroller.h"
#include "actioncontroller.h"
#include "../View/Toolbar/toolbarwidgetnew.h"
#include "toolbarcontroller.h"

class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();

    SelectionController* getSelectionController();
    ActionController* getActionController();
    ToolbarController* getToolbarController();

    void setDefaultIcon(ViewItem* viewItem);
    ViewItem* getModel();
    bool isModelReady();

signals:
    void modelReady(bool);
    void viewItemConstructed(ViewItem* viewItem);
    void viewItemDestructing(int ID, ViewItem *viewItem);

private slots:
    void showToolbar(QPointF pos);
    void setModelReady(bool okay);
    void entityConstructed(EntityAdapter* entity);
    void entityDestructed(EntityAdapter* entity);

private:
    bool _modelReady;
    QHash<int, ViewItem*> viewItems;
    ViewItem* modelItem;

    SelectionController* selectionController;
    ActionController* actionController;
    ToolbarController* toolbarController;

    ToolbarWidgetNew* toolbar;
};

#endif // VIEWCONTROLLER_H
