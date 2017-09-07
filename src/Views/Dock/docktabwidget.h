#ifndef DOCKTABWIDGET_H
#define DOCKTABWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QTabBar>
#include <QScrollArea>
#include <QToolButton>

#include "../../Controllers/ViewController/viewcontroller.h"
#include "dockwidget.h"

class DockTabWidget : public QWidget
{
    Q_OBJECT

public:
    DockTabWidget(ViewController* vc, QWidget * parent = 0);
private:
    void themeChanged();
    void setupLayout();
    void setupDocks();
protected:
    void refreshSize();
    bool eventFilter(QObject *object, QEvent *event);
private:
    void dockActionTriggered(QAction* action);

    ViewController* view_controller = 0 ;

    QMenu* add_part_menu = 0;
    QMenu* deploy_menu = 0;
    
    QScrollArea* deploy_dock = 0;
    QScrollArea* parts_dock = 0;

    QAction* parts_action = 0;
    QAction* deploy_action = 0;
    QToolBar* toolbar = 0;
    
    
    QStackedWidget* stack_widget = 0;
};

#endif // DOCKTABWIDGET_H
