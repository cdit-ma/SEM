#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>

#include "../../Controller/toolbarcontroller.h"
#include "dockwidgetactionitem.h"
#include "dockwidgetitem.h"

class DockWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent = 0);

    ToolActionController::DOCK_TYPE getDockType();

    DockWidgetItem* addItem(QString text);
    DockWidgetActionItem* addActionItem(QAction* action);
    void addActionItems(QList<QAction*> actions);

    void clearDock();
    void updateHeaderText(QString text);

signals:
    void actionClicked(DockWidgetActionItem* action);
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
    QToolButton* backButton;

    QHash<QAction*, DockWidgetActionItem*> childrenActions;
    bool containsHeader;

};

#endif // DOCKWIDGET_H
