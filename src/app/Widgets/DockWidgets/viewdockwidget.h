#ifndef NODEVIEWDOCKWIDGET_H
#define NODEVIEWDOCKWIDGET_H

#include "defaultdockwidget.h"
#include "../../Views/NodeView/nodeview.h"
#include "../../Controllers/SelectionController/selectionhandler.h"

class ViewDockWidget : public DefaultDockWidget
{
    friend class WindowManager;
    Q_OBJECT

protected:
    ViewDockWidget(QString title, QWidget* parent = nullptr, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);

public:
    SelectionHandler* getSelectionHandler();
    NodeView* getNodeView();
    void setWidget(QWidget* widget);

private:
    NodeView* nodeView = nullptr;
};

#endif // NODEVIEWDOCKWIDGET_H
