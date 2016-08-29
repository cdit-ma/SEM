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

    void clearDock();

    ToolActionController::DOCK_TYPE getDockType();

signals:
    void actionClicked(DockActionWidget* action);

public slots:
    void dockActionClicked();

private:
    void setupHeaderLayout();

    ToolActionController* toolActionController;
    ToolActionController::DOCK_TYPE dockType;

    QVBoxLayout* alignLayout;
    QVBoxLayout* mainLayout;
    QVBoxLayout* headerLayout;
    QLabel* descriptionLabel;
    QPushButton* backButton;

    QHash<QAction*, DockActionWidget*> childrenActions;
    bool containsHeader;

};

#endif // DOCKWIDGET_H
