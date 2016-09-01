#ifndef DOCKTABWIDGET_H
#define DOCKTABWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QTabBar>
#include <QScrollArea>
#include <QToolButton>

#include "../../Controller/viewcontroller.h"
#include "dockwidget.h"

class DockTabWidget : public QWidget
{
    Q_OBJECT

public:
    DockTabWidget(ViewController* vc, QWidget * parent = 0);

public slots:
    void themeChanged();
    void selectionChanged();
    void tabClicked(bool checked);

    void dockActionClicked(DockWidgetActionItem *action);
    void dockBackButtonClicked();

private:
    void setupLayout();
    void setupDocks();
    void setupConnections();

    void openRequiredDock(ToolActionController::DOCK_TYPE dt);
    void populateDock(DockWidget* dockWidget, QList<NodeViewItemAction*> actions, bool groupByParent = false);

    //void construct

    DockWidget* getDock(ToolActionController::DOCK_TYPE dt);

    ViewController* viewController;
    ToolActionController* toolActionController;

    DockWidget* partsDock;
    DockWidget* definitionsDock;
    DockWidget* functionsDock;
    DockWidget* hardwareDock;

    QToolButton* partsButton;
    QToolButton* hardwareButton;
    QStackedWidget* stackedWidget;

    QString triggeredAdoptableKind;

};

#endif // DOCKTABWIDGET_H
