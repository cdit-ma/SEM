#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>

#include "../../Controller/toolbarcontroller.h"
#include "dockwidgetparentactionitem.h"
#include "dockwidgetactionitem.h"
#include "dockwidgetitem.h"

class DockWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent = 0);

    ToolActionController::DOCK_TYPE getDockType();

    void addItem(DockWidgetItem* item);
    void addItems(QList<DockWidgetItem*> items);
    void addItems(QList<QAction*> actions);

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
    DockWidgetItem* kindLabel;
    QToolButton* backButton;

    QList<DockWidgetActionItem*> childrenActions;
    QList<DockWidgetParentActionItem*> parentActions;

    QList<DockWidgetItem*> childrenItems;
    QList<QToolBar*> itemToolbars;

    QHash<QAction*, DockWidgetActionItem*> childrenActionHash;
    QHash<QAction*, DockWidgetActionItem*> parentActionHash;
    bool containsHeader;

};

#endif // DOCKWIDGET_H
