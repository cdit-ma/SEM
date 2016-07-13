#ifndef NODEVIEWNEW_H
#define NODEVIEWNEW_H

#include <QGraphicsView>
//#include <QObject>

#include "SceneItems/nodeitemnew.h"
#include "viewitem.h"
#include "Controller/viewcontroller.h"

class NodeViewNew : public QGraphicsView
{
    Q_OBJECT
public:
    NodeViewNew();
    void setViewController(ViewController* viewController);

private slots:


    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);
    void selectionHandler_ItemSelected(ViewItem*item, bool selected);

private slots:
    void item_SetSelected(EntityItemNew* item, bool selected, bool append);
    void item_SetExpanded(EntityItemNew* item, bool expand);

private:
    void nodeViewItem_Constructed(NodeViewItem* item);
    void edgeViewItem_Constructed(EdgeViewItem* item);

    NodeItemNew* getParentNodeItem(NodeViewItem* item);

    EntityItemNew* getEntityItem(int ID);
    EntityItemNew* getEntityItem(ViewItem* item);


private:
    void clearSelection();
    void selectAll();
private:
    QHash<int, EntityItemNew*> guiItems;

    ViewController* viewController;
    SelectionHandler* selectionHandler;

protected:
    void keyPressEvent(QKeyEvent* event);
};

#endif // NODEVIEWNEW_H
