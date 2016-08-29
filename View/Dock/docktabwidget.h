#ifndef DOCKTABWIDGET_H
#define DOCKTABWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QTabBar>
#include <QScrollArea>
#include <QPushButton>

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

    void dockActionClicked(DockActionWidget *action);

private:
    void setupLayout();
    void setupDocks();
    void setupConnections();

    void openRequiredDock(ToolActionController::DOCK_TYPE dt, QString actionKind = "");

    ViewController* viewController;
    ToolActionController* toolActionController;

    QTabBar* tabBar;
    QStackedWidget* dockStack;

    DockWidget* partsDock;
    DockWidget* definitionsDock;
    DockWidget* functionsDock;
    DockWidget* hardwareDock;

    QPushButton* partsButton;
    QPushButton* hardwareButton;
    QStackedWidget* stackedWidget;

};

#endif // DOCKTABWIDGET_H
