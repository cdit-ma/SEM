#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QScrollArea>
#include <QVBoxLayout>

#include "../../Controller/toolbarcontroller.h"
#include "dockactionwidget.h"

class DockWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent = 0);

    ToolActionController::DOCK_TYPE getDockType();

    DockActionWidget* addAction(QAction* action);
    void addActions(QList<QAction*> actions);

    void clearDock();
    void updateHeaderText(QString text);

signals:
    void actionClicked(DockActionWidget* action);
    void backButtonClicked();

public slots:
    void themeChanged();
    void dockActionClicked();

private:
    void setupHeaderLayout();

    ToolActionController* toolActionController;
    ToolActionController::DOCK_TYPE dockType;

    QVBoxLayout* alignLayout;
    QVBoxLayout* mainLayout;
    QVBoxLayout* headerLayout;

    QWidget* mainWidget;
    QLabel* descriptionLabel;
    QPushButton* backButton;

    QHash<QAction*, DockActionWidget*> childrenActions;
    bool containsHeader;

};

#endif // DOCKWIDGET_H
