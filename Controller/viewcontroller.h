#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "entityadapter.h"
#include "../View/viewitem.h"
#include "selectionhandler.h"
#include "../Widgets/New/selectioncontroller.h"
#include "actioncontroller.h"
#include "toolbarcontroller.h"


class ToolbarWidgetNew;
class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();

    QStringList getNodeKinds();
    SelectionController* getSelectionController();
    ActionController* getActionController();
    ToolActionController* getToolbarController();

    void setDefaultIcon(ViewItem* viewItem);
    ViewItem* getModel();
    bool isModelReady();

signals:
    void modelReady(bool);
    void viewItemConstructed(ViewItem* viewItem);
    void viewItemDestructing(int ID, ViewItem *viewItem);
    void triggerAction(QString action);
    void dataChanged(int, QString, QVariant);


    void view_undo();
    void view_redo();




    void canUndo(bool);
    void canRedo(bool);


signals:
    void constructChildNode(int parentID, QString kind, QPointF pos = QPointF());

private slots:
    void table_dataChanged(int ID, QString key, QVariant data);
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
    ToolActionController* toolbarController;

    ToolbarWidgetNew* toolbar;
};

#endif // VIEWCONTROLLER_H
