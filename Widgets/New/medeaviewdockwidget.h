#ifndef MEDEAVIEWDOCKWIDGET_H
#define MEDEAVIEWDOCKWIDGET_H

#include "../../View/nodeviewnew.h"
#include "medeadockwidget.h"
class MedeaViewDockWidget : public MedeaDockWidget
{
public:
    MedeaViewDockWidget(QString title, Qt::DockWidgetArea area = Qt::TopDockWidgetArea);

    NodeViewNew* getNodeView();
    void setWidget(QWidget* widget);

private:
    NodeViewNew* nodeView;
};

#endif // MEDEAVIEWDOCKWIDGET_H
