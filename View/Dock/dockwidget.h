#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "../../Controller/toolbarcontroller.h"
#include "dockactionwidget.h"

class DockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent = 0);

    void addAction(QAction* action);
    void addActions(QList<QAction*> actions);

    ToolActionController::DOCK_TYPE getDockType();

signals:
    void dockActionClicked(QAction* action, ToolActionController::DOCK_TYPE dockType);

public slots:
    void dockActionClicked();

private:
    ToolActionController* toolActionController;
    ToolActionController::DOCK_TYPE dockType;

    QVBoxLayout* alignLayout;
    QVBoxLayout* mainLayout;
    QList<QAction*> childrenActions;

};

#endif // DOCKWIDGET_H
