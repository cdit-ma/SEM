#ifndef NODEVIEWDOCKWIDGET_H
#define NODEVIEWDOCKWIDGET_H

#include "viewdockwidget.h"
#include "../../Views/NodeView/nodeview.h"
#include "../../Controllers/SelectionController/selectionhandler.h"

class NodeViewDockWidget : public ViewDockWidget
{
    friend class WindowManager;
    Q_OBJECT
protected:
    NodeViewDockWidget(QString title, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);
    ~NodeViewDockWidget();
public:


    SelectionHandler* getSelectionHandler();
    NodeView* getNodeView();
    void setWidget(QWidget* widget);

private:
    NodeView* nodeView;
};

#endif // NODEVIEWDOCKWIDGET_H
