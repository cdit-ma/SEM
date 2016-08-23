#ifndef DOCKTABWIDGET_H
#define DOCKTABWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QTabBar>
#include <QScrollArea>
#include <QPushButton>

#include "../../Controller/viewcontroller.h"

class DockTabWidget : public QWidget
{
    Q_OBJECT

public:
    DockTabWidget(ViewController* vc, QWidget * parent = 0);

public slots:
    void themeChanged();
    void selectionChanged();

private:
    void setupLayout();
    void setupDocks();
    void updateDockHeight();

    ViewController* viewController;

    QTabBar* tabBar;
    QStackedWidget* dockStack;
    QToolBar* partsDock;

    QPushButton* partsButton;
    QPushButton* hardwareButton;
    QStackedWidget* stackedWidget;

};

#endif // DOCKTABWIDGET_H
